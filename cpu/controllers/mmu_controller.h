#ifndef __WOLF_CPU_MMU
#define __WOLF_CPU_MMU

#include <stdint.h>

typedef struct WOLF_CPU_MMU WOLF_CPU_MMU,*PWOLF_CPU_MMU;
typedef uint32_t (*PA_MANAGER)(WOLF_CPU_MMU mmu,uint32_t vaddr, uint8_t flags); 
typedef struct {
    uint32_t vaddr;
    uint32_t paddr;
} WOLF_CPU_TLB;

struct WOLF_CPU_MMU{
    WOLF_CPU_TLB* tlb;
    PA_MANAGER* get_pa;
};

void init_cpu_mmu(PWOLF_CPU_MMU* mmu);
#endif