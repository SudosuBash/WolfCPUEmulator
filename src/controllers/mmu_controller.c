#include <controllers/mmu_controller.h>
#include <cpu.h>
typedef struct {
    uint32_t addr;
    uint8_t stat:4;
    uint8_t cache_open:1;

} WOLF_PADDR_GET;


WOLF_PADDR_GET paddr_get(WOLF_CPU* cpu,uint32_t vaddr) {
    WOLF_PADDR_GET res = {0};
    WOLF_MEM_CONTROLLER* controller = cpu->mem_controller;

    uint32_t paddr = vaddr; //指哪打哪
    if(IS_PGO_ON(cpu)) { //尚未完善
        uint16_t offset = vaddr & VADDR_OFFSET_BYTE_MASK;
        uint16_t pte = (vaddr >> VADDR_OFFSET_PTE) & VADDR_OFFSET_PTE_MASK;
        uint16_t pde = (vaddr >> VADDR_OFFSET_PDE);

        uint32_t page_base_address = cpu->spe_regs.pg_mode_base_addr_reg;
        RAM_RD_STATUS stat = controller->rd_ram_4b(controller,page_base_address + BCR_PAGE_TABLE_ITEM_LENG * pde);
        res.stat = BCR_RAM_ERR;
        page_base_address = stat.data.offset4;

        //还得加权限管理，计算页表位置等，这个以后再写
        stat = controller->rd_ram_4b(controller,page_base_address + BCR_PAGE_TABLE_ITEM_LENG * pte);
        res.stat = BCR_RAM_ERR;
        if(res.stat != 0) return res;
    } else {
        res.cache_open = IS_CACHE_ON(cpu);
    }
    res.addr = paddr;
    return res;
}
MMU_STATUS mmu_memory_wr_f(WOLF_CPU_MMU_CONTROLLER* mmu,uint32_t addr,MMU_DATA data) {
    MMU_STATUS res1 = {0};
    WOLF_CPU* cpu = get_parent_struct(mmu,WOLF_CPU,mmu);
    WCPUExecuteResult res = cpu->ex_data_reg;
    WOLF_PADDR_GET paddr = paddr_get(cpu,addr);

    WOLF_CACHE_CONTROLLER* cache = cpu->cache_controller;
    if(paddr.addr < BASE_MMIO_ADDR) { //访问内存 
        
    }
    return res1;
}

MMU_STATUS mmu_memory_rd_f(WOLF_CPU_MMU_CONTROLLER* mmu,uint32_t addr) {
    WOLF_CPU* cpu = get_parent_struct(mmu,WOLF_CPU,mmu);
    MMU_STATUS res1 = {0};
    WOLF_PADDR_GET paddr = paddr_get(cpu,addr);
    if(paddr.stat != 0) {
        res1.stat = paddr.stat;
        return res1;
    }
    
    if(paddr.addr < BASE_MMIO_ADDR) { //访问内存
        //处理缓存部分
        WOLF_CACHE_CONTROLLER* cache = cpu->cache_controller;
        L1_CACHE_RD_GROUP_RES resl1 = cache->rd_l1_groups(cpu->cache1,paddr.addr);
        if(resl1.stat.addr_not_align) {
            res1.stat = BCR_RAM_ERR_ALIGN;
            return res1;
        }
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
        RAM_RD_STATUS stat = cpu->mem_controller->rd_ram(cpu->mem_controller,paddr.addr);
        res1.stat = Through32(stat.dmem_error,BCR_RAM_ERR);
        if(res1.stat != 0) {
            return res1;
        }
        cache->ld_l2_cache(cpu->cache2,paddr.addr,stat.data.offset);
        res1.data = stat.data.offset[resl2.relaAddr >> 2]; //就算未命中，relaAddr也是正常返回的
    } else { //访问 MMIO
        WOLF_CPU_BUS_CONTROLLER* controller = cpu->bus;
        BUS_SEND_DATA bits = {0};
        bits.be = 0b1111;
        bits.read_write = BUS_RW_READ;
        bits = controller->recv_data(controller,paddr.addr,bits);
        if(bits.status == BUS_STATUS_ERROR) {
            res1.stat = BCR_RAM_ERR;
            return res1;
        }
        res1.data = bits.data;
    }

    return res1;
}

WOLF_CPU_MMU_CONTROLLER* init_mmu_controller() {
    WOLF_CPU_MMU_CONTROLLER* mmu = (WOLF_CPU_MMU_CONTROLLER*) malloc(sizeof(WOLF_CPU_MMU_CONTROLLER));
    
    mmu->wr_mmu = mmu_memory_wr_f;
    mmu->rd_mmu = mmu_memory_rd_f;

    return mmu;
}

void free_mmu_controller(WOLF_CPU_MMU_CONTROLLER** controller) {
    if(*controller != NULL) {
        free(*controller);
        *controller = NULL;
    }
}