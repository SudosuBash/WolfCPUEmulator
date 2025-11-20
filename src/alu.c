#include <alu.h>
#include <cpu.h>

static uint8_t get_pf(uint8_t flag) {
    return 
    ((flag >> 7) ^ ((flag >> 6) & 1)) ^
    (((flag >> 5) & 1) ^ ((flag >> 4) & 1)) ^
    (((flag >> 3) & 1) ^ ((flag >> 2) & 1)) ^
    (((flag >> 1) & 1) ^ (flag & 1));
}
static void update_flag(WOLF_CPU* cpu,uint32_t num1,uint32_t num2,uint64_t res,
    uint8_t bit) {
    uint8_t cf = Through8(bit,res >> 32);
    uint8_t sf = res >> 31;
    uint8_t numflag1 = num1 >> 31;
    uint8_t numflag2 = num2 >> 31;
    uint8_t of = Through8(bit,(sf == 1 && numflag1 == 0 && numflag2 == 0) ||
        (sf == 0 && numflag1 == 1 && numflag2 == 1));
    uint8_t res_low8 = res & 0xff;
    uint8_t pf = get_pf(res_low8);
    uint8_t zf = res == 0;
    cpu->spe_regs.scr = (of << SCR_OF_FLAG) | (cf << SCR_CF_FLAG) | 
        (zf << SCR_ZF_FLAG) | (sf << SCR_SF_FLAG) | (pf << SCR_PF_FLAG);
}
uint32_t alu_add_op(PWOLF_ALU* alu,uint32_t idata1,uint32_t idata2,uint8_t sgn) {
    WOLF_CPU* wcpu = get_parent_struct(alu,WOLF_CPU,alu);
    uint64_t res = ((uint64_t)(idata1) & 0xffffffff) + ((uint64_t)(idata2) & 0xffffffff) + (uint64_t)sgn;
    update_flag(wcpu,idata1,idata2,res,1);
    return (uint32_t)(res);
}

uint32_t alu_and_op(PWOLF_ALU* alu,uint32_t idata1,uint32_t idata2) {
    WOLF_CPU* wcpu = get_parent_struct(alu,WOLF_CPU,alu);
    uint32_t res = (idata1) & (idata2);
    update_flag(wcpu,idata1,idata2,res,0);
    return (res);
}

uint32_t alu_or_op(PWOLF_ALU* alu,uint32_t idata1,uint32_t idata2) {
    WOLF_CPU* wcpu = get_parent_struct(alu,WOLF_CPU,alu);
    uint32_t res = (idata1) | (idata2);
    update_flag(wcpu,idata1,idata2,res,0);
    return (res);
}

uint32_t alu_xor_op(PWOLF_ALU* alu,uint32_t idata1,uint32_t idata2) {
    WOLF_CPU* wcpu = get_parent_struct(alu,WOLF_CPU,alu);
    uint32_t res = (idata1) ^ (idata2);
    update_flag(wcpu,idata1,idata2,res,0);
    return (res);
}
uint32_t alu_neg_op(PWOLF_ALU* alu,uint32_t idata1,uint32_t idata2) {
    WOLF_CPU* wcpu = get_parent_struct(alu,WOLF_CPU,alu);
    uint32_t res = ~(idata1);
    update_flag(wcpu,idata1,idata2,res,0);
    return (res);
}

uint32_t alu_not_op(PWOLF_ALU* alu,uint32_t idata1,uint32_t idata2) {
    WOLF_CPU* wcpu = get_parent_struct(alu,WOLF_CPU,alu);
    uint32_t res = -(idata1);
    update_flag(wcpu,idata1,idata2,res,0);
    return (res);
}

uint32_t alu_ml_op(PWOLF_ALU* alu,uint32_t idata1,uint32_t idata2,uint8_t isAlu) {
    WOLF_CPU* wcpu = get_parent_struct(alu,WOLF_CPU,alu);
    uint32_t res = idata1 << idata2;
    update_flag(wcpu,idata1,idata2,res,0);
    return res;
}

uint32_t alu_mr_op(PWOLF_ALU* alu,uint32_t idata1,uint32_t idata2,uint8_t isAlu) {
    WOLF_CPU* wcpu = get_parent_struct(alu,WOLF_CPU,alu);
    uint32_t res = 
        Through8(isAlu,((int32_t)idata1) >> idata2) |
        Through8(!isAlu,idata1 >> idata2);
    update_flag(wcpu,idata1,idata2,res,0);
    return res;
}
// uint64_t alu_mul_op(PWOLF_ALU* alu,uint32_t idata1,uint32_t idata2,uint8_t sgn) {
//     WOLF_CPU* wcpu = get_parent_struct(alu,WOLF_CPU,alu);
//     uint64_t res = (uint64_t)(idata1) * (uint64_t)(idata2) + (uint64_t)sgn;
//     update_flag(wcpu,idata1,idata2,res);
//     return (uint64_t)(res);
// }

// uint32_t alu_div_op(PWOLF_ALU* alu,uint64_t idata1,uint32_t idata2,uint8_t sgn) {
//     WOLF_CPU* wcpu = get_parent_struct(alu,WOLF_CPU,alu);
//     uint64_t res = ((uint64_t)sgn + (uint64_t)(idata1)) / (uint64_t)(idata2);
//     update_flag(wcpu,idata1,idata2,res);
//     return (uint32_t)(res);
// }

WOLF_ALU* init_alu() {
    WOLF_ALU* alu = (WOLF_ALU*)malloc(sizeof(WOLF_ALU));
    if(alu == NULL) return NULL;
    alu->add_operate = alu_add_op;
    alu->and_operate = alu_and_op;
    alu->neg_operate = alu_neg_op;
    alu->not_operate = alu_not_op;
    alu->or_operate = alu_or_op;
    alu->xor_operate = alu_xor_op;
    alu->ml_operate = alu_ml_op;
    alu->mr_operate = alu_mr_op;
    return alu;
}

void free_alu(WOLF_ALU** alu) {
    if(*alu != NULL) {
        free(*alu);
    }
}