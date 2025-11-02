#ifndef _WOLFREGS
#define _WOLFREGS

#include <stdint.h>

typedef struct {
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t r13;
    uint32_t r14;
} CPU_General_Registers;

typedef struct {
    uint16_t bcr;
    uint16_t scr;
    uint8_t flags;
} CPU_Special_Registers;

typedef struct {
    uint32_t mep;
    uint32_t mval;
    uint64_t memode;
    uint64_t mimode;
    uint16_t mreason;
    uint8_t mmode;
    uint32_t mpc;
} CPU_Ecall_Registers;
#endif