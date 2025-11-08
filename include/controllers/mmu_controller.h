#ifndef __WOLF_CPU_MMU
#define __WOLF_CPU_MMU

#include <stdint.h>

#define MMU_CONVERT_TO_EREASON(ecode) (1 << 2) | (ecode)
#define BCR_PAGE_TABLE_ITEM_LENG 32

#define BCR_RAM_ERR_OK 0
#define BCR_RAM_ERR 1
#define BCR_RAM_ERR_ALIGN 2
#define BCR_RAM_ERR_ACCESS_DENIED 3

#define VADDR_OFFSET_PTE 12
#define VADDR_OFFSET_PDE 22
#define VADDR_OFFSET_BYTE_MASK (1 << VADDR_OFFSET_PTE) - 1
#define VADDR_OFFSET_PTE_MASK (1 << VADDR_OFFSET_PDE) - 1

typedef struct WOLF_CPU_MMU WOLF_CPU_MMU_CONTROLLER,*PWOLF_CPU_MMU_CONTROLLER;
// typedef struct {
//     uint32_t vaddr;
//     uint32_t paddr;
// } WOLF_CPU_TLB;
//思考再三，我决定去掉TLB的设计
//TLB本意是当页表缓存，加速查表速度。但是问题是这个TLB反倒因为"C语言串行"的因素拖慢模拟器速度
typedef struct {
    uint32_t data;
    uint8_t be:4;
    uint8_t status:2;
} MMU_DATA;

typedef struct {
    uint8_t stat:4;
    uint32_t data;
} MMU_STATUS;

typedef MMU_STATUS (*mmu_memory_rd)(PWOLF_CPU_MMU_CONTROLLER* mmu,uint32_t addr);
typedef MMU_STATUS (*mmu_memory_wr)(PWOLF_CPU_MMU_CONTROLLER* mmu,uint32_t addr,MMU_DATA data);

struct WOLF_CPU_MMU{
    mmu_memory_wr wr_mmu;
    mmu_memory_rd rd_mmu;
};

WOLF_CPU_MMU_CONTROLLER* init_mmu_controller();
void free_mmu_controller(PWOLF_CPU_MMU_CONTROLLER* controller);
#endif