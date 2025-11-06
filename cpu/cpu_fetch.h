#ifndef __WOLF_CPU_DECODER
#define __WOLF_CPU_DECODER

#include <stdint.h>
#include "cpu/logics/logic_alg.h"


#define ICODE_MOV 0b000001
#define ICODE_ALU 0b000010
#define ICODE_MLMR 0b000011
#define ICODE_JMP_I 0b100000
#define ICODE_JMP_II 0b100001
#define ICODE_ECALL 0b100010
#define ICODE_OCALL 0b100011
#define ICODE_RSCR 0b110001
#define ICODE_LSCR 0b110010
#define ICODE_LPGR 0b110011
#define ICODE_LIER 0b110100
#define ICODE_RET 0b100100

#define ICODE_RET_EXFUNC 0x01
#define ICODE_IRET_EXFUNC 0x10
#define ICODE_ERET_EXFUNC 0x11

#define CMD_ITYPE 0
#define CMD_RTYPE 1

typedef struct {
    uint8_t irtype:1;
    uint8_t icode:6;
    uint8_t reg1:5;
    uint8_t reg2:5;
    uint16_t idata;
    uint8_t aluExFunc:3;
    uint8_t jmpExCond:4;
    uint8_t ExFlag:5;
    uint8_t noexception:1;//指令应该继续执行吗?
} WCPUFetchData;

WCPUFetchData fetchData(WOLF_CPU* cpu);
#endif