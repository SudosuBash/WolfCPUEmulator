#include "cpu_mem.h"
#include "cpu.h"

WCPUMemResult memory(WOLF_CPU* cpu) {
    WCPUExecuteResult res = cpu->ex_data_reg;
    WCPUMemResult mem_res = {0};
    WOLF_CPU_MMU_CONTROLLER* controller = cpu->mmu;
    if(!res.noexception) 
        goto CPU_MEM_END_STATUS;
    uint8_t icode = res.icode;
    uint8_t exflag = res.ExFlag;


    mem_res.destReg = res.destReg;
    mem_res.ExCond = res.ExCond;
    mem_res.icode = res.icode;
    mem_res.valC = res.valC;
    mem_res.valC_Extended = res.valC_Extended;
    mem_res.noexception = res.noexception;
    mem_res.ExFlag = res.ExFlag;
    mem_res.ExFunc = res.ExFunc;
CPU_MEM_END_STATUS:
    cpu->mem_data_reg = mem_res;
    return mem_res;
}

