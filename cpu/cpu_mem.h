#ifndef _WOLF_CPU_MEM
#define _WOLF_CPU_MEM

#include <stdint.h>
typedef struct {
    uint32_t op_result;
    uint32_t op_result2; //用于乘除法
    uint32_t mem_rd_value;
    uint8_t icode;
    uint8_t destReg;
    uint8_t ExCond:3;
    uint8_t ExFunc:5;
    uint8_t ExFlag:5;
    uint8_t noexception:1;//指令应该继续执行吗?
} WCPUMemResult;
#endif