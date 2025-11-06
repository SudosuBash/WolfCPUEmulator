#include "cpu_execute.h"

WCPUExecuteResult execute(WOLF_CPU* cpu) {
    WCPUDecodedData data = cpu->id_data_reg;
    uint8_t icode = data.icode;
    WCPUExecuteResult res = {0};
    if(!data.noexception) 
        goto CPU_EXEC_END_STATUS;
    res.noexception = data.noexception;
    switch (icode) {
        case ICODE_ALU: {
            //用组合逻辑会导致条件为false时这段依然执行，很繁琐
            uint8_t sgn = data.ExFlag & ALU_EXFUNC_SGN_MASK;
            uint8_t ex_code = data.ExFlag & ALU_EXFUNC_NEG_MASK;
            uint8_t final_ex_func = (ex_code << ALU_EXFUNC_BITS) | ex_code;
            uint32_t res1 = 0;
            uint32_t res2 = 0; //用于乘除
            switch (final_ex_func) //加速
            {
            case ALU_FUN_CODE_ADD:
                res1 = cpu->alu->add_operate(cpu->alu,data.valA,data.valB,sgn);
                break;
            case ALU_FUN_CODE_AND:
                res1 = cpu->alu->and_operate(cpu->alu,data.valA,data.valB);
                break;
            case ALU_FUN_CODE_SUB:
                res1 = cpu->alu->add_operate(cpu->alu,data.valA,-data.valB,sgn);
                break;
            case ALU_FUN_CODE_OR:
                res1 = cpu->alu->or_operate(cpu->alu,data.valA,data.valB);
                break;
            case ALU_FUN_CODE_XOR:
                res1 = cpu->alu->xor_operate(cpu->alu,data.valA,data.valB);
                break;
            default: 
                ecall(cpu->ecall_controller,ECALL_MACHINE_PROBLEM,EREASON_FOR_UNSUPPORTED_ALU_FUNC); //异常触发函数
                res.noexception = 0;
                break;
            }
            res.op_result = res1;
            break;
        }
        case ICODE_MLMR: {
            uint8_t ml_mr = data.ExFlag & ALU_EXFUNC_MLMR_MASK;
            uint32_t res1 = 0;
            switch (ml_mr)
            {
            case ALU_EXFUNC_ML:
                res1 = cpu->alu->ml_operate(cpu->alu,data.valA,data.valB);
                break;
            case ALU_EXFUNC_MR:
                res1 = cpu->alu->mr_operate(cpu->alu,data.valA,data.valB);
                break;
            }
            res.op_result = res1;
            break;
        }

        case ICODE_JMP_I:
            res.op_result = data.valA + data.valB;
            break;
        case ICODE_JMP_II:
            res.op_result = data.valB;
            break;
        case ICODE_RET: {
            uint8_t flag = data.ExFlag;
            switch(flag) {
                case ICODE_RET_EXFUNC:
                    res.op_result = data.valA + 4;
                    break;
            }
            break;
        }

        case ICODE_ECALL:
            res.op_result = data.valA - 4;
            break;
        case ICODE_OCALL:
            res.op_result = data.valA - 4;
            break;            
    }
    res.icode = data.icode;
    res.ExCond = data.ExCond;
    res.destReg = data.destRegs;
    res.ExFunc = data.ExFunc;
    res.ExFlag = data.ExFlag;
CPU_EXEC_END_STATUS:
    cpu->ex_data_reg = res;
    return res;
}