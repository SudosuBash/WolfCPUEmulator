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

#define CMD_ITYPE 0
#define CMD_RTYPE 1

typedef struct {
    uint8_t icode:6;
    uint8_t reg1:5;
    uint8_t reg2:5;
    uint16_t idata;
    uint8_t aluExFunc:3;
    uint8_t jmpExCond:3;
    uint8_t ExFlag:5;
} WCPUDecodedData;

WCPUDecodedData decodeData(uint32_t fetch);
#endif