#include "cpu_fetch.h"
#include <cpu/controllers/ecall.h>
#include "cpu.h"

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
      (icode == ICODE_ECALL) ||
      (icode == ICODE_RSCR) ||
      (icode == ICODE_LSCR) ||
      (icode == ICODE_LPGR) ||
      (icode == ICODE_LIER);
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

static inline uint8_t getExFunc(uint8_t type,uint8_t icode,uint32_t data) {
    uint8_t cond = (
      icode == ICODE_ALU ||
      icode == ICODE_MLMR 
    );
    uint8_t val = Through8(cond,(data >> 16) & 0b1111);
    cond = (ICODE_RET == icode);
    uint8_t val2 = Through8(cond,(data >> 20) & 0b11111);

    return Through8(type ^ 1,val | val2);
}

static inline uint8_t getExCond(uint8_t type,uint8_t icode,uint32_t data) {
    uint8_t val = Through8(icode == ICODE_JMP_I,(data >> 16) & 0b1111);
    uint8_t val2 = Through8(icode == ICODE_JMP_II, (data >> 20) & 0b111);
    return Through8(type ^ 1,val | val2);
}

static inline uint8_t getExFlag(uint8_t type,uint8_t icode,uint32_t data) {
    uint8_t cond = icode == ICODE_MOV || ICODE_LIER;
    uint8_t val1 = Through8(cond,(icode >> 16) & 0b1111);
    cond = icode == ICODE_ALU || ICODE_MLMR;
    uint8_t val2 = Through8(cond,(icode >> 12) & 0b11111);

    return Through8(type^1, val1 | val2);
}
WCPUFetchData decodeData(WOLF_CPU* cpu,uint32_t fetch) {
    WCPUFetchData result;
    uint8_t type = fetch >> 31;
    uint8_t icode = (fetch >> 25) & 0b0111111;
    uint32_t data = fetch & 0x01ffffff;

    result.irtype = fetch & 0x7fffffff;
    result.icode = icode;
    result.reg1 = getLRegisterA(icode, data);
    result.reg2 = getLRegisterB(type,icode,data);
    result.idata = getIData(type, icode, data);
    result.aluExFunc = getExFunc(type,icode,data);
    result.jmpExCond = getExCond(type,icode,data);
    result.ExFlag = getExFlag(type,icode,data);
    
    uint8_t invalid = (icode != ICODE_ALU 
        && icode != ICODE_ECALL 
        && icode != ICODE_ERET_EXFUNC 
        && icode != ICODE_IRET_EXFUNC
        && icode != ICODE_JMP_I
        && icode != ICODE_JMP_II 
        && icode != ICODE_LIER
        && icode != ICODE_LPGR
        && icode != ICODE_LSCR
        && icode != ICODE_MLMR
        && icode != ICODE_MOV
        && icode != ICODE_OCALL
        && icode != ICODE_RET
        && icode != ICODE_RSCR); //电路中直接6-64然后对那些未使用的引脚做or就好
    result.noexception = 1;
    if(Through8(result.irtype ^ 1,invalid)) {
        ecall(cpu->ecall_controller,ECALL_MACHINE_PROBLEM,EREASON_FOR_UNSUPPORTED_ICODE); //调用 ecall 函数，代表出现问题
        result.noexception = 0;
    }
    return result;
} 