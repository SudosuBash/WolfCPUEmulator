#include <cpu.h>

void writeback(WOLF_CPU* cpu) {
    if(cpu->temp_data_reg.noexception == 0) {
        cpu->temp_data_reg.noexception = 0;
        goto WB_ERR_END;
        
    }
    uint8_t icode = cpu->temp_data_reg.icode;
    
    switch(icode) {
        case ICODE_MOV: {
            if(IS_RTYPE(cpu->temp_data_reg.irtype) && ICODE_EXFLAG_MOV_MEM1(cpu->temp_data_reg.ExFlag)) {
                break;
            }
            uint8_t exflag = cpu->temp_data_reg.ExFlag >> 3;
            uint8_t bec = ICODE_EXFLAG_MOV_BE(cpu->temp_data_reg.ExFlag >> 3);
            write_reg_val(cpu,cpu->temp_data_reg.destReg, DATA32_MASK_BE(
                cpu->gen_regs.r[cpu->temp_data_reg.destReg],
                cpu->temp_data_reg.valC,
                ICODE_EXFLAG_MOV_BE(cpu->temp_data_reg.ExFlag >> 3)
            ));
            break;
        }
        case ICODE_MLMR:
        case ICODE_ALU: {
            write_reg_val(cpu,cpu->temp_data_reg.destReg,DATA32_MASK_BE(
                cpu->gen_regs.r[cpu->temp_data_reg.destReg],
                cpu->temp_data_reg.valC,
                ICODE_EXFLAG_MOV_BE(0)
            ));
            break;
        }
        case ICODE_LEBR:
        case ICODE_LEPV:
        case ICODE_LIBR:
        case ICODE_LIPV:
        case ICODE_LPGR:
        case ICODE_LBCR:
            write_reg_val(cpu,cpu->temp_data_reg.destReg, cpu->temp_data_reg.valB);
            break;
        //4个指令全部传送valB
        case ICODE_RERE:
        case ICODE_ECALL:
        case ICODE_OCALL:
        case ICODE_PUSH:
        case ICODE_PUSHF:
        case ICODE_ZWC:
        case ICODE_RET:
        case ICODE_REARG:
            cpu->gen_regs.r[cpu->temp_data_reg.destReg] = cpu->temp_data_reg.valB;
            break;
        case ICODE_POPF:
        case ICODE_POP:
            cpu->gen_regs.r[cpu->temp_data_reg.destReg2] = cpu->temp_data_reg.valB;
            cpu->gen_regs.r[cpu->temp_data_reg.destReg] = cpu->temp_data_reg.valC;
            break;
    }
WB_ERR_END:
}