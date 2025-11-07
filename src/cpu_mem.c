#include <cpu.h>

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
    switch(res.icode) {
        case ICODE_RET: {
            if(res.ExFunc != 0) break; //确保普通的ret
            MMU_STATUS stat = controller->rd_mmu(controller,res.valA);
            if(stat.stat != 0) {
                ecall(cpu->ecall_controller,ECALL_MACHINE_PROBLEM,MMU_CONVERT_TO_EREASON(stat.stat));
                res.noexception = 0;
                goto CPU_MEM_END_STATUS;
            }
            mem_res.valC = stat.data;
            //M[valC] <- valA;
            break;
        }
        case ICODE_OCALL: {
            MMU_DATA data = {0};
            data.be = 0b1111;
            controller->wr_mmu(controller,res.valC,data);
            break;            
        }
    }
CPU_MEM_END_STATUS:
    cpu->mem_data_reg = mem_res;
    return mem_res;
}

