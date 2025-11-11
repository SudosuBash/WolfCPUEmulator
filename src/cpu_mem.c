#include <cpu.h>

void memory(WOLF_CPU* cpu) {
    WCPUExecuteResult res = cpu->ex_data_reg;
    WCPUMemResult mem_res = {0};
    WOLF_CPU_MMU_CONTROLLER* controller = cpu->mmu;
    if(!res.noexception) 
        goto CPU_MEM_END_STATUS;
    uint8_t icode = res.icode;
    uint8_t exflag = res.ExFlag;
    
    uint8_t be = exflag >> 3;
    uint8_t be1b = Through8(be==1,0b1);
    uint8_t be2b = Through8(be==2,0b11);
    uint8_t be4b = Through8(be==0,0b1111);
    uint8_t finalBe=be1b | be2b | be4b;
    mem_res.destReg = res.destReg;
    mem_res.ExCond = res.ExCond;
    mem_res.icode = res.icode;

    mem_res.valC = res.valC;
    mem_res.valC_Extended = res.valC_Extended;
    mem_res.noexception = res.noexception;
    mem_res.ExFlag = res.ExFlag;
    mem_res.ExFunc = res.ExFunc;

    uint32_t wr_data = Through32((
        icode == ICODE_OCALL ||
        icode == ICODE_PUSH
    ),res.valC);
    uint32_t wr_addr = Through32((
        icode == ICODE_OCALL ||
        icode == ICODE_PUSH
    ),res.valB);
    switch(res.icode) {
        case ICODE_RET: {
            if(res.ExFunc != 0) break; //确保普通的ret
            MMU_STATUS stat = controller->rd_mmu(&controller,res.valA,be);
            if(stat.stat != BCR_RAM_ERR_OK) {
                cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,MMU_CONVERT_TO_EREASON(stat.stat));
                res.noexception = 0;
                goto CPU_MEM_END_STATUS;
            }
            mem_res.valC = stat.data;
            //M[valC] <- valA;
            break;
        }
        case ICODE_PUSH:
        case ICODE_OCALL: {
            MMU_DATA data = {0};
            data.be = finalBe;
            data.data = wr_data;
            MMU_STATUS stat = controller->wr_mmu(&cpu->mmu,wr_addr,data);
            if(stat.stat != BCR_RAM_ERR_OK) {
                cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,MMU_CONVERT_TO_EREASON(stat.stat));
                res.noexception = 0;
                goto CPU_MEM_END_STATUS;
            }
            break;            
        }
    }
CPU_MEM_END_STATUS:
    cpu->mem_data_reg = mem_res;
}

