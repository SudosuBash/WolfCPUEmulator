#include "mmu_controller.h"
#include <cpu/cpu.h>

#define BCR_PAGE_TABLE_ITEM_LENG 32
#define BCR_STAT_OK 0
#define BCR_STAT_ACCESS_DENIED 1
#define BCR_RAM_ERR_MASK 0b10
#define BCR_RAM_ERR_OFFSET 1

#define VADDR_OFFSET_PTE 12
#define VADDR_OFFSET_PDE 22
#define VADDR_OFFSET_BYTE_MASK (1 << VADDR_OFFSET_PTE) - 1
#define VADDR_OFFSET_PTE_MASK (1 << VADDR_OFFSET_PDE) - 1
typedef struct {
    uint32_t addr;
    uint8_t stat:2;
    uint8_t flag:4;
} WOLF_PADDR_GET;


WOLF_PADDR_GET paddr_get(WOLF_CPU* cpu,uint32_t vaddr) {
    WOLF_PADDR_GET res = {0};
    uint8_t pg_open = (cpu->spe_regs.bcr & BCR_PGO_MASK) >> 1;
    WOLF_MEM_CONTROLLER* controller = cpu->mem_controller;

    uint32_t paddr = vaddr; //指哪打哪
    if(pg_open) {
        uint16_t offset = vaddr & VADDR_OFFSET_BYTE_MASK;
        uint16_t pte = (vaddr >> VADDR_OFFSET_PTE) & VADDR_OFFSET_PTE_MASK;
        uint16_t pde = (vaddr >> VADDR_OFFSET_PDE);

        uint32_t page_base_address = cpu->spe_regs.pg_mode_base_addr_reg;
        RAM_RD_STATUS stat = controller->rd_ram_4b(controller,page_base_address + BCR_PAGE_TABLE_ITEM_LENG * pde);
        res.stat |= (stat.dmem_error << BCR_RAM_ERR_OFFSET) & (BCR_RAM_ERR_MASK);
        if(res.stat != 0) return res;
        page_base_address = stat.val32;
    }
}
void mmu_memory_wr_f(WOLF_CPU_MMU_CONTROLLER* mmu,uint32_t addr) {
    WOLF_CPU* cpu = get_parent_struct(mmu,WOLF_CPU,mmu);
    WCPUExecuteResult res = cpu->ex_data_reg;

    

}
MMU_STATUS mmu_memory_rd_f(WOLF_CPU_MMU_CONTROLLER* mmu,uint32_t addr) {

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