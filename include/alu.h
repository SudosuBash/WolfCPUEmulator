#ifndef __WOLF_ALU
#define __WOLF_ALU

#include <stdint.h>

typedef struct WOLF_ALU WOLF_ALU,*PWOLF_ALU;
typedef uint32_t (*ALU_OPERATE_FN)(PWOLF_ALU* alu,uint32_t idata1,uint32_t idata2,uint8_t sgn);
typedef uint32_t (*ALU_OPERATE_FN_NO_SGN)(PWOLF_ALU* alu,uint32_t idata1,uint32_t idata2);
typedef uint32_t (*ALU_MLMR_OPERATE)(PWOLF_ALU* alu,uint32_t idata1,uint32_t idata2,uint8_t isAlu);

struct WOLF_ALU {
    ALU_OPERATE_FN add_operate;
    ALU_OPERATE_FN mul_operate;
    ALU_MLMR_OPERATE ml_operate;
    ALU_MLMR_OPERATE mr_operate;
    ALU_OPERATE_FN_NO_SGN and_operate;
    ALU_OPERATE_FN_NO_SGN or_operate;
    ALU_OPERATE_FN_NO_SGN xor_operate;
    ALU_OPERATE_FN_NO_SGN neg_operate;
};
#endif