#include <cpu.h>

#define ZWC_16(data) ( (Through32(((data) & 0x00000080) >> 7,(data) | 0xffffff00) |\
                Through32(!(((data) & 0x0000080) >> 7), res.valA & 0x000000ff)) )
#define ZWC_32(data) ( (Through32(((data) & 0x00008000) >> 15,(data) | 0xffff0000) |\
                Through32(!(((data) & 0x0008000) >> 15), (data) & 0x0000ffff)) )
                               
void execute(WOLF_CPU* cpu) {
    WCPUDecodedData data = cpu->id_data_reg;
    uint8_t icode = data.icode;
    WCPUExecuteResult res = {0};
    if(!data.noexception) 
        goto CPU_EXEC_END_STATUS;
    res.noexception = data.noexception;
    res.valB = data.valB;
    res.valC = data.valC;
    res.valA = data.valA;
    switch (icode) {
        case ICODE_MOV: {
            uint8_t i1_mem = ICODE_EXFLAG_MOV_MEM1(data.ExFlag);
            uint8_t i2_mem = ICODE_EXFLAG_MOV_MEM1(data.ExFlag);
            if(i1_mem && i2_mem && IS_RTYPE(data.irtype)) {
                cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,EREASON_FOR_UNSUPPORTED_ICODE);
                goto CPU_EXEC_END_STATUS;
            }
            uint32_t f_val = Through32(i1_mem,res.valA) | Through32(i2_mem || (!i1_mem && !i2_mem),res.valB);
            res.valC = Through32(IS_RTYPE(data.irtype),f_val) |
                Through32(IS_ITYPE(data.irtype),res.valC);
            break;
        }
        case ICODE_ALU: {
            //用组合逻辑会导致条件为false时这段依然执行，很繁琐
            uint8_t sgn = (data.ExFlag >> ALU_EXFLAG_SGN_MASK) & 1;
            uint8_t ex_code = (data.ExFlag >> ALU_EXFLAG_NEG_MASK) & 1;
            uint8_t final_ex_func = (ex_code << ALU_EXFUNC_BITS) | data.ExFunc;
            uint8_t need_op = (data.ExFlag >> ALU_EXFLAG_OPR_MASK) & 1;
            uint32_t res1 = 0;
            uint32_t res2 = 0; //用于乘除
            switch (final_ex_func) //加速
            {
            case ALU_FUN_CODE_ADD:
                res1 = cpu->alu->add_operate(&cpu->alu,data.valA,data.valC,sgn);
                break;
            case ALU_FUN_CODE_AND:
                res1 = cpu->alu->and_operate(&cpu->alu,data.valA,data.valC);
                break;
            case ALU_FUN_CODE_SUB:
                res1 = cpu->alu->add_operate(&cpu->alu,data.valA,~data.valC,sgn ^ 1);
                break;
            case ALU_FUN_CODE_OR:
                res1 = cpu->alu->or_operate(&cpu->alu,data.valA,data.valC);
                break;
            case ALU_FUN_CODE_XOR:
                res1 = cpu->alu->xor_operate(&cpu->alu,data.valA,data.valC);
                break;
            default: 
                cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,EREASON_FOR_UNSUPPORTED_ALU_FUNC); //异常触发函数
                res.noexception = 0;
                break;
            }
            res.valC = Mux32(need_op,res.valC,res1);
            break;
        }
        case ICODE_MLMR: {
            uint8_t ml_mr = data.ExFlag & ALU_EXFUNC_MLMR_MASK;
            uint8_t is_alu = GET_MLMR_EXFUNC_ALU(data.ExFlag);
            uint32_t res1 = 0;
            switch (ml_mr)
            {
            case ALU_EXFUNC_ML:
                res1 = cpu->alu->ml_operate(&cpu->alu,data.valA,data.valB,is_alu);
                break;
            case ALU_EXFUNC_MR:
                res1 = cpu->alu->mr_operate(&cpu->alu,data.valA,data.valB,is_alu);
                break;
            }
            res.valC = res1;
            break;
        }
        case ICODE_JMP: {
            res.valC = data.valA + data.valC;
            break;
        }
        case ICODE_RET: {
            uint8_t func= data.ExFunc;
            switch(func) {
                case ICODE_RET_EXFUNC:
                    res.valB = data.valB + 4;
            }
            break;
        }
        case ICODE_OCALL:
            res.valB = data.valB - 4; //WB写回
            res.valC = data.valA + data.valC;
            break;
        case ICODE_PUSHF:
        case ICODE_PUSH:
            res.valC = data.valA;
            res.valB = data.valB - 4;  //WB写回
            break;
        case ICODE_POPF:
        case ICODE_POP:
            res.valB = res.valB + 4;
            break;
        case ICODE_ZWC: {
            uint32_t targetVal = Through32(data.ExFlag & 1, ZWC_16(res.valA)) |
                Through32(!data.ExFlag & 1,ZWC_32(res.valA));
            res.valB = targetVal;
            break;
        }   
    }
    res.icode = data.icode;
    res.ExCond = data.ExCond;
    res.destReg = data.destReg;
    res.destReg2 = data.destReg2;
    res.ExFunc = data.ExFunc;
    res.ExFlag = data.ExFlag;
    res.irtype = data.irtype;
CPU_EXEC_END_STATUS:
    cpu->ex_data_reg = res;
}