#ifndef __WOLF_CPU_MMU
#define __WOLF_CPU_MMU

#include <stdint.h>

#define BCR_PGO_MASK 0b10

typedef struct WOLF_CPU_MMU WOLF_CPU_MMU_CONTROLLER,*PWOLF_CPU_MMU;
typedef uint32_t (*PA_MANAGER)(WOLF_CPU_MMU_CONTROLLER* mmu,uint32_t vaddr, uint8_t flags); 

// typedef struct {
//     uint32_t vaddr;
//     uint32_t paddr;
// } WOLF_CPU_TLB;
//思考再三，我决定去掉TLB的设计
//TLB本意是当页表缓存，加速查表速度。但是问题是这个TLB反倒因为"C语言串行"的因素拖慢模拟器速度

typedef void (*mmu_memory_wr)(WOLF_CPU_MMU_CONTROLLER* mmu,uint32_t addr);
typedef MMU_DATA (*mmu_memory_rd)(WOLF_CPU_MMU_CONTROLLER* mmu,uint32_t addr);

typedef struct {
    uint8_t stat:4;
    uint32_t data;
} MMU_STATUS;
struct WOLF_CPU_MMU{
    // WOLF_CPU_TLB tlb[8];
    mmu_memory_wr wr_mmu;
    mmu_memory_rd rd_mmu;
};
typedef struct {
    uint32_t data;
    uint8_t be:4;
    uint8_t status:2;
} MMU_DATA;

void init_cpu_mmu(PWOLF_CPU_MMU* mmu);
WOLF_CPU_MMU_CONTROLLER* init_mmu_controller();
#endif