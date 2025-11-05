#ifndef _WOLF_CPU_DECODE
#define _WOLF_CPU_DECODE

#include <stdint.h>
typedef struct {
    uint32_t valA;
    uint32_t valB;
    uint16_t idata;
    uint8_t ExCond:3;
    uint8_t ExFunc:5;
    
} WCPUDecodedData;
#endif