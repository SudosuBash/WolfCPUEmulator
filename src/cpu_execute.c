#include <cpu.h>

#define ZWC_16(data) ( (Through32(((data) & 0x00000080) >> 7,(data) | 0xffffff00) |\
                Through32(!(((data) & 0x0000080) >> 7), (data) & 0x000000ff)) )
#define ZWC_32(data) ( (Through32(((data) & 0x00008000) >> 15,(data) | 0xffff0000) |\
                Through32(!(((data) & 0x0008000) >> 15), (data) & 0x0000ffff)) )

void execute(WOLF_CPU* cpu) {
    uint8_t icode = cpu->temp_data_reg.icode;
    if(!cpu->temp_data_reg.noexception) 
        goto CPU_EXEC_END_STATUS;
    switch (icode) {
        case ICODE_MOV: {
            uint8_t i1_mem = ICODE_EXFLAG_MOV_MEM1(cpu->temp_data_reg.ExFlag);
            uint8_t i2_mem = ICODE_EXFLAG_MOV_MEM2(cpu->temp_data_reg.ExFlag);
            uint8_t bit = cpu->temp_data_reg.ExFlag >> 3;
            if(i1_mem && i2_mem && IS_RTYPE(cpu->temp_data_reg.irtype)) {
                cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,EREASON_FOR_UNSUPPORTED_ICODE,cpu->temp_data_reg.instruction);
                goto CPU_EXEC_END_STATUS;
            }
            uint32_t f_val = Through32(i1_mem,cpu->temp_data_reg.valA) | Through32(i2_mem || (!i1_mem && !i2_mem),cpu->temp_data_reg.valB);
            cpu->temp_data_reg.valC = Mux32(bit == EXFLAG_OPERATE_2_BITS_HIGH,cpu->temp_data_reg.valC,cpu->temp_data_reg.valC << 16);
            cpu->temp_data_reg.valC = Through32(IS_RTYPE(cpu->temp_data_reg.irtype),f_val) |
                Through32(IS_ITYPE(cpu->temp_data_reg.irtype),cpu->temp_data_reg.valC);
            break;
        }
        case ICODE_ALU: {
            //用组合逻辑会导致条件为false时这段依然执行，很繁琐
            uint8_t sgn = (cpu->temp_data_reg.ExFlag >> ALU_EXFLAG_SGN_MASK) & 1;
            uint8_t ex_code = (cpu->temp_data_reg.ExFlag >> ALU_EXFLAG_EXALU_MASK) & 1;
            uint8_t final_ex_func = (ex_code << ALU_EXFUNC_BITS) | cpu->temp_data_reg.ExFunc;
            uint8_t need_op = (cpu->temp_data_reg.ExFlag >> ALU_EXFLAG_OPR_MASK) & 1;
            uint32_t res1 = 0;
            uint32_t res2 = 0; //用于乘除
            uint8_t cf = GET_SCR_CF_FLAG(cpu->spe_regs.scr);
            switch (final_ex_func) //加速
            {
            case ALU_FUN_CODE_ADD:
                res1 = cpu->alu->add_operate(&cpu->alu,cpu->temp_data_reg.valA,cpu->temp_data_reg.valC,sgn & cf);
                break;
            case ALU_FUN_CODE_SUB:

                res1 = cpu->alu->add_operate(&cpu->alu,cpu->temp_data_reg.valA,~cpu->temp_data_reg.valC,(sgn & cf) ^ 1);
                break;

            case ALU_FUN_CODE_AND:
                res1 = cpu->alu->and_operate(&cpu->alu,cpu->temp_data_reg.valA,cpu->temp_data_reg.valC);
                break;
            case ALU_FUN_CODE_OR:
                res1 = cpu->alu->or_operate(&cpu->alu,cpu->temp_data_reg.valA,cpu->temp_data_reg.valC);
                break;
            case ALU_FUN_CODE_XOR:
                res1 = cpu->alu->xor_operate(&cpu->alu,cpu->temp_data_reg.valA,cpu->temp_data_reg.valC);
                break;
            case ALU_FUN_CODE_NEG:
                res1 = cpu->alu->neg_operate(&cpu->alu,cpu->temp_data_reg.valA,0);
                break;
            case ALU_FUN_CODE_NOT:
                res1 = cpu->alu->not_operate(&cpu->alu,cpu->temp_data_reg.valA,0);
                break;
            default: 
                cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,EREASON_FOR_UNSUPPORTED_ALU_FUNC,cpu->temp_data_reg.instruction); //异常触发函数
                cpu->temp_data_reg.noexception = 0;
                break;
            }
            cpu->temp_data_reg.valC = Mux32(need_op,cpu->temp_data_reg.valA,res1);
            break;
        }
        case ICODE_MLMR: {
            uint8_t ml_mr = cpu->temp_data_reg.ExFlag & ALU_EXFUNC_MLMR_MASK;
            uint8_t is_alu = GET_MLMR_EXFUNC_ALU(cpu->temp_data_reg.ExFlag);
            uint32_t res1 = 0;
            switch (ml_mr)
            {
            case ALU_EXFUNC_ML:
                res1 = cpu->alu->ml_operate(&cpu->alu,cpu->temp_data_reg.valA,cpu->temp_data_reg.valC,is_alu);
                break;
            case ALU_EXFUNC_MR:
                res1 = cpu->alu->mr_operate(&cpu->alu,cpu->temp_data_reg.valA,cpu->temp_data_reg.valC,is_alu);
                break;
            }
            cpu->temp_data_reg.valC = res1;
            break;
        }
        case ICODE_JMP: {
            int16_t f2 = (int16_t)(cpu->temp_data_reg.valC & 0xffff);
            cpu->temp_data_reg.valC = cpu->temp_data_reg.valA + (int16_t)(cpu->temp_data_reg.valC & 0xffff);
            break;
        }
        case ICODE_RET: {
            uint8_t func= cpu->temp_data_reg.ExFunc;
            switch(func) {
                case ICODE_RET_EXFUNC:
                    cpu->temp_data_reg.valB = cpu->temp_data_reg.valB + 4;
            }
            break;
        }
        case ICODE_OCALL:
            cpu->temp_data_reg.valB = cpu->temp_data_reg.valB - 4; //WB写回
            cpu->temp_data_reg.valC = cpu->temp_data_reg.valA + (int16_t)(cpu->temp_data_reg.valC & 0xffff);
            break;
        case ICODE_PUSHF:
        case ICODE_PUSH:
            cpu->temp_data_reg.valC = cpu->temp_data_reg.valA;
            cpu->temp_data_reg.valB = cpu->temp_data_reg.valB - 4;  //WB写回
            break;
        case ICODE_POPF:
        case ICODE_POP:
            cpu->temp_data_reg.valB = cpu->temp_data_reg.valB + 4;
            break;
        case ICODE_ZWC: {
            uint32_t targetVal = Through32(cpu->temp_data_reg.ExFlag & 1, ZWC_16(cpu->temp_data_reg.valA)) |
                Through32(!(cpu->temp_data_reg.ExFlag & 1),ZWC_32(cpu->temp_data_reg.valA));
            cpu->temp_data_reg.valB = targetVal;
            break;
        }   
    }

CPU_EXEC_END_STATUS:
    return;
}