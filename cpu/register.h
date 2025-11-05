#ifndef _WOLFREGS
#define _WOLFREGS

#include <stdint.h>

#define MAX_GEN_REGISTER_COUNT 14
typedef struct {
    uint32_t r[MAX_GEN_REGISTER_COUNT];
    // uint32_t r1;
    // uint32_t r2;
    // uint32_t r3;
    // uint32_t r4;
    // uint32_t r5;
    // uint32_t r6;
    // uint32_t r7;
    // uint32_t r8;
    // uint32_t r9;
    // uint32_t r10;
    // uint32_t r11;
    // uint32_t r12;
    // uint32_t r13;
    // uint32_t r14;
} CPU_General_Registers;

typedef struct {
    uint16_t bcr;
    uint16_t scr;
    uint8_t flags;
    uint32_t pg_mode_base_addr_reg;
} CPU_Special_Registers;

typedef struct {
    uint32_t mep; //存储当前pc
    uint64_t memode; //对应中断的特权级(x位)
    uint64_t mimode; //对应异常特权级(x位)
    uint16_t mreason; //原因
    uint8_t mmode; //特权级
    uint32_t mpc; //中断/异常基址
} CPU_Ecall_Registers;
#endif