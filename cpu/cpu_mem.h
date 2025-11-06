#ifndef _WOLF_CPU_MEM
#define _WOLF_CPU_MEM

#include <stdint.h>
typedef struct {
    uint8_t icode;
    uint8_t destReg;
    uint32_t valC_Extended;//用于乘除法
    uint32_t valC;
    uint32_t valB;
    uint8_t ExCond:3;
    uint8_t ExFunc:5;
    uint8_t ExFlag:5;
    uint8_t noexception:1;//指令应该继续执行吗?
} WCPUMemResult;
WCPUMemResult memory(WOLF_CPU* cpu);
#endif