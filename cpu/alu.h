#ifndef __WOLF_ALU
#define __WOLF_ALU

#include <stdint.h>

typedef uint32_t (*ALU_OPERATE_FN)(WOLF_ALU* alu,uint32_t idata1,uint32_t idata2);

typedef struct {
    ALU_OPERATE_FN add_operate;
    ALU_OPERATE_FN sub_operate;
    ALU_OPERATE_FN mul_operate;
    ALU_OPERATE_FN div_operate;
    ALU_OPERATE_FN ml_operate;
    ALU_OPERATE_FN mr_operate;
} WOLF_ALU;

#endif