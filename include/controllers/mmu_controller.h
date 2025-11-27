#ifndef __WOLF_CPU_MMU
#define __WOLF_CPU_MMU

#include <stdint.h>

#define MMU_CONVERT_TO_EREASON(ecode) (1 << 3) | ((ecode) - 1)
#define BCR_PAGE_TABLE_ITEM_LENG 4

#define BCR_RAM_ERR_OK 0
#define BCR_RAM_ERR_DMEM_ERROR 1
#define BCR_RAM_ERR_ALIGN 2
#define BCR_RAM_ERR_BUS_ERROR 3
#define BCR_RAM_ERR_REG_TIMEOUT 5
#define BCR_PAGE_STAT_ACCESS_DENIED 6
#define BCR_PAGE_STAT_NOT_EXEC 7
#define BCR_PAGE_STAT_PAGE_FAULT 8

#define VADDR_OFFSET_PTE 12
#define VADDR_OFFSET_PDE 22
#define VADDR_OFFSET_BYTE_MASK (1 << VADDR_OFFSET_PTE) - 1
#define VADDR_OFFSET_PTE_MASK (1 << VADDR_OFFSET_PDE) - 1

#define MMU_CONTROLLER_REGS 12
#define MMU_CONTROLLER_NOW_BUSDEVICE_REGA 0x0 //2Bits
#define MMU_CONTROLLER_NOW_BUSDEVICE_CMD 0x2 //1Bit
#define MMU_CONTROLLER_NOW_BUSDEVICE_VAL_REGA 0x4 //4Bit
#define MMU_CONTROLLER_NOW_BUSDEVICE_NEED_SPACE_REGA 0x8 //2Bit
#define MMU_CONTROLLER_NOW_BUSDEVICE_VENDOR_ID_REGA 0xa //2Bit

#define MMU_CONTROLLER_CMD_SET_IRQNUM 0x3 //1Bit
#define MMU_CONTROLLER_CMD_SET_BASE_ADDR 0x2

#define PDE_EXECUTE 0b10
#define PDE_RW 0b1

#define PDE_ACCESS_FOR_KERNEL 0
#define PDE_ACCESS_FOR_USER 1

#define PDE_GET_OFFSET(pde) ((pde) >> 12)
#define PDE_GET_PREPARED(pde) (((pde) >> 11) & 1)
#define PDE_GET_ACCESS(pde) (((pde) >> 10) & 1)
#define PDE_GET_DIRTY(pde) (((pde) >> 9) & 1)
#define PDE_GET_PERMISSION(pde) (((pde) >> 8) & 1)



// typedef struct {
//     uint32_t offset:20;
//     uint8_t prepared:1;
//     uint8_t access:1;
//     uint8_t dirty:1;
//     uint8_t permission:1; //可读写?可执行?
//     uint8_t reserved:8; //保留
// } RAM_PDE_DATA;
typedef uint8_t BCR_STATUS;

typedef struct {
    uint8_t code:1; //执行代码 or 访问数据?
    uint8_t read:1; //读 or 写?
} CPU_MMU_BEHAVIOR;

typedef struct WOLF_CPU_MMU WOLF_CPU_MMU_CONTROLLER,*PWOLF_CPU_MMU_CONTROLLER;

//32bits
// typedef struct {
//     uint32_t vaddr;
//     uint32_t paddr;
// } WOLF_CPU_TLB;
//思考再三，我决定去掉TLB的设计
//TLB本意是当页表缓存，加速查表速度。但是问题是这个TLB反倒因为"C语言串行"的因素拖慢模拟器速度
typedef struct {
    uint8_t data[4];
    uint8_t be:4;
    uint8_t status:2;
} MMU_DATA;


typedef struct {
    uint8_t stat:4;
    uint8_t data[4];
} MMU_STATUS;

typedef MMU_STATUS (*mmu_memory_rd)(PWOLF_CPU_MMU_CONTROLLER* mmu,uint32_t addr,uint8_t be,uint8_t fetch);
typedef MMU_STATUS (*mmu_memory_wr)(PWOLF_CPU_MMU_CONTROLLER* mmu,uint32_t addr,MMU_DATA data);

struct WOLF_CPU_MMU{
    uint8_t regs[MMU_CONTROLLER_REGS];
    mmu_memory_wr wr_mmu;
    mmu_memory_rd rd_mmu;
};

WOLF_CPU_MMU_CONTROLLER* init_mmu_controller();
void free_mmu_controller(PWOLF_CPU_MMU_CONTROLLER* controller);
#endif