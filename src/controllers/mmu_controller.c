#include <controllers/mmu_controller.h>
#include <cpu.h>
typedef struct {
    uint32_t addr;
    uint8_t stat:4;
    uint8_t cache_open:1;
} WOLF_PADDR_GET;

static const uint32_t bios_code[512 / 4] = {
    0x0210ffff,
    0x06000010,
    0x0210ffff,
    0x04004100
    // 0x03c0f0ff,
    // 0xc6201000,
    // 0,
    // 0,
    // 0,
    // 0,
    // 0xc8200000
};

WOLF_PADDR_GET paddr_get(WOLF_CPU* cpu,uint32_t vaddr) {
    WOLF_PADDR_GET res = {0};
    PWOLF_MEM_CONTROLLER* controller = &cpu->mem_controller;

    uint32_t paddr = vaddr; //指哪打哪
    if(IS_PGO_ON(cpu)) { //尚未完善
        uint16_t offset = vaddr & VADDR_OFFSET_BYTE_MASK;
        uint16_t pte = (vaddr >> VADDR_OFFSET_PTE) & VADDR_OFFSET_PTE_MASK;
        uint16_t pde = (vaddr >> VADDR_OFFSET_PDE);

        uint32_t page_base_address = cpu->spe_regs.pg_mode_base_addr_reg;
        RAM_RD_STATUS stat = (*controller)->rd_ram_4b(controller,page_base_address + BCR_PAGE_TABLE_ITEM_LENG * pde,0b1111);
        res.stat = BCR_RAM_ERR;
        page_base_address = stat.data.offset4;

        //还得加权限管理，计算页表位置等，这个以后再写
        stat = (*controller)->rd_ram_4b(controller,page_base_address + BCR_PAGE_TABLE_ITEM_LENG * pte,0b1111);
        res.stat = BCR_RAM_ERR;
        if(res.stat != 0) return res;
    }
    res.addr = paddr; 
    return res;
}

MMU_STATUS mmu_memory_wr_f(PWOLF_CPU_MMU_CONTROLLER* pmmu,uint32_t addr,MMU_DATA data) {
    MMU_STATUS res1 = {0};
    WOLF_CPU_MMU_CONTROLLER* controller = *pmmu;
    WOLF_CPU* cpu = get_parent_struct(pmmu,WOLF_CPU,mmu);
    WCPUExecuteResult res = cpu->ex_data_reg;
    WOLF_PADDR_GET paddr = paddr_get(cpu,addr);

    if((paddr.addr & (BE_ALIGN(data.be))) != 0) { //物理内存地址要求4字节对齐
        res1.stat = BCR_RAM_ERR_ALIGN;
        return res1;          
    }

    WOLF_CACHE_CONTROLLER* cache = cpu->cache_controller;
    if(paddr.addr < BASE_MMIO_ADDR) { //访问内存 
        RAM_WR_STATUS stat = cpu->mem_controller->wr_ram(&cpu->mem_controller,paddr.addr,data.data,data.be);
        if(stat.dmem_error) {
            res1.stat = BCR_RAM_ERR;
            return res1;
        }
        RAM_RD_STATUS stat2 = cpu->mem_controller->rd_ram(&cpu->mem_controller,paddr.addr);
        if(stat2.dmem_error) {
            res1.stat = BCR_RAM_ERR;
            return res1;
        }
        cache->ld_l2_cache(cpu->cache2,paddr.addr,stat2.data.offset);
        uint32_t relaAddr = ((addr & L2_OFFSET_MASK) - (addr & (L1_OFFSET_MASK))) >> 2;
        cache->ld_l1_cache(cpu->cache1,paddr.addr,(&stat2.data.offset[relaAddr]));
    } else if(paddr.addr < BASE_MMU_ADDR) { //访问MMIO
        WOLF_CPU_BUS_CONTROLLER* controller = cpu->bus;
        BUS_SEND_DATA bits = {0};
        bits.be = data.be;
        bits.read_write = BUS_RW_WRITE;
        bits.data = data.data;
        uint8_t status = controller->send_data(&cpu->bus,paddr.addr,bits);
        if(status == BUS_STATUS_ERROR) {
            res1.stat = BCR_RAM_ERR;
            return res1;
        }
        if(status == BUS_STATUS_TIMEOUT) {
            res1.stat = BCR_RAM_ERR_REG_TIMEOUT;
            return res1;
        }
        res1.stat = BCR_RAM_ERR_OK;
        return res1;
    } else if(paddr.addr < BASE_BIOS_ADDR) { //访问MMU，简化逻辑不单独设计一个元件了
        uint32_t pos_addr = (paddr.addr - BASE_MMU_ADDR) >> 2;
        if(pos_addr >= MMU_CONTROLLER_REGS * sizeof(uint32_t)) {
            res1.stat = BCR_RAM_ERR_REG_OUT_OF_RANGE;
            return res1;
        }
        controller->regs[pos_addr] = DATA32_MASK_BE(controller->regs[pos_addr],data.data,
            BE_NOT_ALIGN4_GET(paddr.addr & 3,data.be));
        //写入对应的寄存器，但是实际上的话是这样的
        //paddr.addr & 3 获取余数
        //data.be是对应地址要写入的be
        //获取到的be就是相对于pos_addr的be

        uint32_t cmd = controller->regs[MMU_CONTROLLER_NOW_BUSDEVICE_CMD];
        uint32_t device_num = controller->regs[MMU_CONTROLLER_NOW_BUSDEVICE_REGA];
        if(device_num > MAX_BUS_DEVICE) return res1;
        //简化逻辑就不增加对外接口了，真实电路应该加在Bus加一个对外访问函数用于模拟对外接口
        controller->regs[MMU_CONTROLLER_NOW_BUSDEVICE_REGA] = cpu->bus->devices[device_num]->vendor_id; 
        //更新设备的总线位置，读取对应的vendor_id
        //此寄存器只读，CPU对这个寄存器的写入操作不会触发异常，但是结果无效,NEED_SPACE同此
        controller->regs[MMU_CONTROLLER_NOW_BUSDEVICE_NEED_SPACE_REGA] = cpu->bus->devices[device_num]->need_space;
        //更新设备的总线位置，读取对应的need_space
        switch(cmd) {
            case MMU_CONTROLLER_CMD_SET_BASE_ADDR: { //设置基地址
                if(controller->regs[MMU_CONTROLLER_NOW_BUSDEVICE_VAL_REGA] >= BASE_MMIO_ADDR
                && controller->regs[MMU_CONTROLLER_NOW_BUSDEVICE_VAL_REGA] < BASE_MMU_ADDR) {
                    cpu->bus->devices[device_num]->base_address = controller->regs[MMU_CONTROLLER_NOW_BUSDEVICE_VAL_REGA];
                }
                break;
            }
            case MMU_CONTROLLER_CMD_SET_IRQNUM: { //设置中断号
                cpu->bus->devices[device_num]->irq_number = controller->regs[MMU_CONTROLLER_NOW_BUSDEVICE_VAL_REGA] & 0xff;
                //0~63位
                break;
            }
        }
        controller->regs[MMU_CONTROLLER_NOW_BUSDEVICE_VAL_REGA] = 0; 
        //更新base_address
        //一次设备枚举包含两个阶段，一个是更新设备总线位置，一个是更新设备的bar

    } else { //尝试读写BIOS，直接ACCESS DENIED
        res1.stat = BCR_RAM_ERR_ACCESS_DENIED;
    }
    return res1;
}

MMU_STATUS mmu_memory_rd_f(PWOLF_CPU_MMU_CONTROLLER* pmmu,uint32_t addr,uint8_t be) {
    WOLF_CPU* cpu = get_parent_struct(pmmu,WOLF_CPU,mmu);
    WOLF_CPU_MMU_CONTROLLER* controller = *pmmu;
    MMU_STATUS res1 = {0};
    WOLF_PADDR_GET paddr = paddr_get(cpu,addr);
    if(paddr.stat != 0) {
        res1.stat = paddr.stat;
        return res1;
    }
    if(paddr.addr & (BE_ALIGN(be)) != 0) { 
        res1.stat = BCR_RAM_ERR_ALIGN;
        return res1;          
    }
    if(paddr.addr < BASE_MMIO_ADDR) { //访问内存
        //处理缓存部分
        WOLF_CACHE_CONTROLLER* cache = cpu->cache_controller;
        L1_CACHE_RD_GROUP_RES resl1 = cache->rd_l1_groups(cpu->cache1,paddr.addr);
        if(resl1.stat.hit) {
            res1.data = resl1.offset[resl1.relaAddr >> 2];
            return res1;
        }
        
        L2_CACHE_RD_GROUP_RES resl2 = cache->rd_l2_groups(cpu->cache2,paddr.addr);
        if(resl2.stat.hit) {
            res1.data = resl2.offset[resl2.relaAddr >> 2];
            cache->ld_l1_cache(cpu->cache1,paddr.addr,(&resl2.offset[resl2.relaAddr >> 2]));
            return res1;
        }
        RAM_RD_STATUS stat = cpu->mem_controller->rd_ram(&cpu->mem_controller,paddr.addr);
        res1.stat = Through32(stat.dmem_error,BCR_RAM_ERR);
        if(res1.stat != 0) {
            return res1;
        }
        cache->ld_l2_cache(cpu->cache2,paddr.addr,stat.data.offset);
        res1.data = stat.data.offset[resl2.relaAddr >> 2]; //就算未命中，relaAddr也是正常返回的
    } else if(paddr.addr < BASE_MMU_ADDR) { //访问 MMIO
        WOLF_CPU_BUS_CONTROLLER* controller = cpu->bus;
        BUS_SEND_DATA bits = {0};
        bits.be = be;
        bits.read_write = BUS_RW_READ;
        bits = controller->recv_data(&cpu->bus,paddr.addr,bits);
        if(bits.status == BUS_STATUS_ERROR) {
            res1.stat = BCR_RAM_ERR;
            return res1;
        }
        if(bits.status == BUS_STATUS_TIMEOUT) {
            res1.stat = BCR_RAM_ERR_REG_TIMEOUT;
            return res1;
        }
        res1.data = bits.data;
    } else if(paddr.addr < BASE_BIOS_ADDR) { //访问MMU控制器的设备获取部分
        uint32_t pos_addr = paddr.addr - BASE_MMU_ADDR;
        if(pos_addr >= MMU_CONTROLLER_REGS * sizeof(uint32_t)) {
            res1.stat = BCR_RAM_ERR_REG_OUT_OF_RANGE;
            return res1;
        }
        //摆大烂设计
        //读取MMU的设备获取寄存器必须4字节对齐，否则触发对齐异常
        if(be != 0b1111) { 
            res1.stat = BCR_RAM_ERR_ALIGN;
            return res1;
        }
        uint32_t data = controller->regs[pos_addr >> 2];
        res1.data = controller->regs[pos_addr >> 2];
    } else {
//还没有实现对应逻辑，为了能够执行，暂时先用临时一个uint8_t数组代替
        res1.data = bios_code[(paddr.addr - BASE_BIOS_ADDR) >>2];
    }

    return res1;
}

WOLF_CPU_MMU_CONTROLLER* init_mmu_controller() {
    WOLF_CPU_MMU_CONTROLLER* mmu = (WOLF_CPU_MMU_CONTROLLER*) calloc(1,sizeof(WOLF_CPU_MMU_CONTROLLER));
    mmu->wr_mmu = mmu_memory_wr_f;
    mmu->rd_mmu = mmu_memory_rd_f;
    return mmu;
}

void free_mmu_controller(PWOLF_CPU_MMU_CONTROLLER* controller) {
    if(*controller != NULL) {
        free(*controller);
        *controller = NULL;
    }
}