#include <cpu.h>

void memory(WOLF_CPU* cpu) {
    WCPUExecuteResult res = cpu->ex_data_reg;
    WCPUMemResult mem_res = {0};
    WOLF_CPU_MMU_CONTROLLER* controller = cpu->mmu;
    if(!res.noexception) 
        goto CPU_MEM_END_STATUS;
    uint8_t icode = res.icode;
    uint8_t exflag = res.ExFlag;
    
    uint8_t be = ICODE_EXFLAG_MOV_BE(exflag >> 3);
    mem_res.destReg = res.destReg;
    mem_res.destReg2 = res.destReg2;
    mem_res.ExCond = res.ExCond;
    mem_res.icode = res.icode;

    mem_res.irtype = res.irtype;
    mem_res.valC = res.valC;
    mem_res.valC_Extended = res.valC_Extended;
    mem_res.noexception = res.noexception;
    mem_res.ExFlag = res.ExFlag;
    mem_res.ExFunc = res.ExFunc;
    mem_res.valB = res.valB;
    uint32_t wr_data = Through32((
        icode == ICODE_OCALL ||
        icode == ICODE_PUSH
    ),res.valC);
    uint32_t wr_addr = Through32((
        icode == ICODE_OCALL ||
        icode == ICODE_PUSH
    ),res.valB);

    uint32_t rd_addr = Through32((
        icode == ICODE_RET
    ),res.valA) |
    Through32((
        icode == ICODE_POP
    ),res.valB - 4);
    switch(res.icode) {
        case ICODE_MOV: {
            MMU_STATUS stat = {.stat = 0,.data = 0};
            if(!IS_RTYPE(res.irtype)) break;
            if(ICODE_EXFLAG_MOV_MEM1(res.ExFlag)) {
                uint32_t addr1 = Through32((
                        icode == ICODE_MOV &&
                        ICODE_EXFLAG_MOV_MEM1(res.ExFlag)
                    ),res.valC);
                MMU_DATA data = {.data = res.valB,.be = be};
                stat = controller->wr_mmu(&cpu->mmu,addr1,data);
            } else if(ICODE_EXFLAG_MOV_MEM2(res.ExFlag)) {
                uint32_t addr2 = Through32((
                        icode == ICODE_MOV &&
                        ICODE_EXFLAG_MOV_MEM2(res.ExFlag)
                    ),res.valC);
                stat = controller->rd_mmu(&cpu->mmu,addr2,be);
                mem_res.valC = stat.data;
            }
            if(stat.stat != BCR_RAM_ERR_OK) {
                cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,MMU_CONVERT_TO_EREASON(stat.stat));
                res.noexception = 0;
                goto CPU_MEM_END_STATUS;
            }
            break;
        }
        case ICODE_RET: 
            if(res.ExFunc != 0) break; //确保普通的ret
            //M[valC] <- valA;
        case ICODE_POPF:
        case ICODE_POP: {
            MMU_STATUS stat = controller->rd_mmu(&cpu->mmu,rd_addr,be);
            if(stat.stat != BCR_RAM_ERR_OK) {
                cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,MMU_CONVERT_TO_EREASON(stat.stat));
                res.noexception = 0;
                goto CPU_MEM_END_STATUS;
            }
            mem_res.valC = stat.data;
            break;
        }
        case ICODE_PUSHF:
        case ICODE_PUSH:
        case ICODE_OCALL: {
            MMU_DATA data = {0};
            data.be = be;
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

