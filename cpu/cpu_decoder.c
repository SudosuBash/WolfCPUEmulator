#include "cpu_decoder.h"

static inline uint8_t getLRegisterB(uint8_t type,uint8_t icode, uint32_t data) {
    uint8_t icodeA = 
      (icode == ICODE_MOV) ||
      (icode == ICODE_ALU) ||
      (icode == ICODE_MLMR);
    uint8_t regB = (data >> 15) & 0x1f;
    return Through8(type,Through8(icodeA,regB));
}
static inline uint8_t getLRegisterA(uint8_t icode,uint32_t data) {
    uint8_t icodeA = 
      (icode == ICODE_MOV) ||
      (icode == ICODE_ALU) || 
      (icode == ICODE_MLMR) ||
      (icode == ICODE_ECALL);
    uint8_t regA = (data >> 20) & 0x1f;
    return Through8(icodeA, regA);
}

static inline uint32_t getIData(uint8_t type, uint8_t icode,uint32_t data) {
    
    uint8_t cond = (icode == ICODE_MOV || icode == ICODE_JMP_I || icode == ICODE_OCALL);
    uint32_t icmd1_4_7_val = Through32(cond,(data & 0xffff));
    cond = (icode == ICODE_ALU);
    uint32_t icmd2_val = Through32(cond,(data & 0x0fff));
    cond = (icode == ICODE_MLMR);
    uint32_t icmd3_val = Through32(cond,(data & 0x1f));
    cond = (icode == ICODE_JMP_II);
    uint32_t icmd5_val = Through32(cond,(data & 0xfffff));
    cond = (icode == ICODE_ECALL);
    uint32_t icmd6_val = Through32(cond,(data & 0xff)); 
    uint32_t final = icmd6_val | icmd5_val | icmd2_val | icmd3_val | icmd1_4_7_val;
    uint32_t result1 = Through32(type ^ 0b00000001,final);
    return result1;
}
WCPUDecodedData decodeData(uint32_t fetch) {
    WCPUDecodedData result;
    uint8_t type = fetch >> 31;
    uint8_t icode = (fetch >> 25) & 0b0111111;
    uint32_t data = fetch & 0x01ffffff;

    result.icode = icode;
    result.reg1 = getLRegisterA(icode, data);
    result.reg2 = getLRegisterB(type,icode,data);
    result.idata = getIData(type, icode, data);
    return result;
} 