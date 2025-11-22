#ifndef _WOLFREGS
#define _WOLFREGS

#include <stdint.h>

#define MAX_GEN_REGISTER_COUNT 15
typedef struct {
    uint32_t r[MAX_GEN_REGISTER_COUNT];
} CPU_General_Registers;

typedef struct {
    uint16_t bcr;
    uint16_t scr;
    uint8_t flags;
    uint32_t pg_mode_base_addr_reg;
} CPU_Special_Registers;

typedef struct {
    uint32_t mep; //存储当前pc
    uint64_t mmode; //对应异常特权级(x位)
    uint16_t mreason; //原因
    uint8_t mprevmode; //特权级
    uint32_t mpc; //中断/异常基址
    uint32_t eargs; //异常参数
} CPU_Ecall_Registers;
#endif