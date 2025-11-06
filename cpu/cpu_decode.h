#ifndef _WOLF_CPU_DECODE
#define _WOLF_CPU_DECODE

#include <stdint.h>

typedef struct {
    uint8_t icode;
    uint8_t destRegs;
    uint32_t valA;
    uint32_t valB;
    uint32_t valC;
    uint8_t ExCond:4;
    uint8_t ExFunc:5;
    uint8_t ExFlag:5;
    uint8_t noexception:1;//指令应该继续执行吗?
} WCPUDecodedData;
WCPUDecodedData decode(WOLF_CPU* cpu);
#endif