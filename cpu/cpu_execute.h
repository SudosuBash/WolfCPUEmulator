#ifndef _WOLF_CPU_EXECUTE
#define _WOLF_CPU_EXECUTE

#include "cpu.h"
#include <stdint.h>
#include "cpu_decode.h"


#define ALU_FUN_CODE_ADD 0x1
#define ALU_FUN_CODE_MUL 0x2
#define ALU_FUN_CODE_AND 0x3
#define ALU_FUN_CODE_OR 0x4
#define ALU_FUN_CODE_XOR 0x5
#define ALU_FUN_CODE_NEG 0x6
#define ALU_FUN_CODE_SUB 0b1001
#define ALU_FUN_CODE_DIV 0b1010

#define ALU_EXFUNC_BITS 3

#define ALU_EXFUNC_NEG_MASK 0b1
#define ALU_EXFUNC_SGN_MASK 0b10

#define ALU_EXFUNC_MLMR_MASK 0x1
#define ALU_EXFUNC_ML 0x0
#define ALU_EXFUNC_MR 0x1


typedef struct {
    uint32_t op_result;
    uint32_t op_result2; //用于乘除法
    uint8_t icode;
    uint8_t destReg;
    uint8_t ExCond:3;
    uint8_t ExFunc:5;
    uint8_t ExFlag:5;
    uint8_t noexception:1;//指令应该继续执行吗?
}WCPUExecuteResult;

WCPUExecuteResult execute(WOLF_CPU* cpu);
#endif