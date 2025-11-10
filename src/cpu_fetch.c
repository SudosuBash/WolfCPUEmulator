#include <controllers/ecall.h>
#include <cpu.h>

static inline uint8_t getExCond(uint8_t type,uint8_t icode,uint32_t data) {
    uint8_t val = Through8(icode == ICODE_JMP,(data >> 16) & 0b1111);
    return Through8(type ^ 1,val);
}

static inline uint8_t getLRegisterB(uint8_t type,uint8_t icode, uint32_t data) {
    uint8_t icodeB = 
        (icode == ICODE_ECALL);
    uint8_t regB = (data >> 20) & 0x1f;
    uint8_t regB2 = CPU_REG_ECALL_ECBASE;
    //Ecall: rB = spe.ecall_base
    
    uint8_t icodeC = (icode == ICODE_OCALL || icode == ICODE_RET);
    uint8_t regB3 = CPU_REG_RSP;
    //Call: rB = rsp

    uint8_t icodeD = (icode == ICODE_LSCR);
    uint8_t regB4 = CPU_REG_SPE_SCR;
    //Lscr: rB = SCR

    uint8_t icodeE = (icode == ICODE_LPGR);
    uint8_t regB5 = CPU_REG_SPE_PGBASE;
    //Lpgr: rb = PGBASE

    uint8_t icodeF = (icode == ICODE_LEBR);
    uint8_t regB6 = CPU_REG_ECALL_ECBASE;
    //Lebr: rb = ECBASE
    uint8_t icodeG = (icode == ICODE_LIBR);
    uint8_t regB7 = CPU_REG_ECALL_ICBASE;
    //Libr: rb = ICBASE
    uint8_t icodeH = (icode == ICODE_LERE);
    uint8_t regB8 = CPU_REG_ECALL_REASON;

    uint8_t icodeI = (icode == ICODE_LIRE);
    uint8_t regB9 = CPU_REG_IRQ_REASON;

    return Through8(!type,Through8(icodeB,regB2)) |
        Through8(!type,Through8(icodeC,regB3)) |
        Through8(!type,Through8(icodeD,regB4)) |
        Through8(!type,Through8(icodeE,regB5)) |
        Through8(!type,Through8(icodeF,regB6)) |
        Through8(!type,Through8(icodeG,regB7)) |
        Through8(!type,Through8(icodeH,regB8)) |
        Through8(!type,Through8(icodeI,regB9)) ;
}


static inline uint8_t getExFlag(uint8_t type,uint8_t icode,uint32_t data) {
    uint8_t cond = icode == ICODE_MOV;
    uint8_t movExFlag = (data >> 16) & 0b1111;
    uint8_t val1 = Through8(cond,movExFlag);
    
    cond = icode == ICODE_ALU || ICODE_MLMR;
    uint8_t val2 = Through8(cond,(icode >> 12) & 0b1111);

    cond = icode == ICODE_OCALL;
    uint8_t val3 = Through8(cond,(icode >> 16) & 0b1111);
    return Through8(type^1, val1 | val2 | val3);
}

static inline uint8_t getLRegisterA(uint8_t type,uint8_t icode,uint32_t data) {
    uint8_t icodeA = 
      (icode == ICODE_MOV) ||
      (icode == ICODE_ALU) || 
      (icode == ICODE_MLMR) ||
      (icode == ICODE_ECALL) ||
      (icode == ICODE_RSCR) ||
      (icode == ICODE_LSCR) ||
      (icode == ICODE_LPGR) ||
      (icode == ICODE_LEBR) ||
      (icode == ICODE_LIBR) ||
      (icode == ICODE_LIRE) ||
      (icode == ICODE_LERE);
    uint8_t regA = (data >> 20) & 0x1f;
    
    uint8_t icodeB = 
        (icode == ICODE_RET);
    uint8_t regB = CPU_REG_RSP;

    uint8_t icodeC = 
        (icode == ICODE_OCALL);
    uint8_t r1_on = EXFLAG_R1_ON(getExFlag(type,icode,data));
    uint8_t regC = Through8(r1_on,regA) |
        Through8(!r1_on,CPU_REG_PC);

    uint8_t icodeD = (icode == ICODE_JMP);
    r1_on = EXCOND_R1_ON(getExCond(type,icode,data));
    uint8_t regD = Through8(r1_on,regA) |
         Through8(!r1_on,CPU_REG_PC);
    
    
    return Through8(icodeA, regA) |
        Through8(icodeB,regB) |
        Through8(icodeC,regC) |
        Through8(icodeD,regD);
}

static inline uint32_t getIData(uint8_t type, uint8_t icode,uint32_t data) {
    uint8_t cond = (icode == ICODE_MOV || icode == ICODE_JMP || icode == ICODE_OCALL);
    uint32_t icmd1_4_7_val = Through32(cond,(data & 0xffff));
    cond = (icode == ICODE_ALU);
    uint32_t icmd2_val = Through32(cond,(data & 0x0fff));
    cond = (icode == ICODE_MLMR);
    uint32_t icmd3_val = Through32(cond,(data & 0x1f));

    cond = (icode == ICODE_ECALL);
    uint32_t icmd6_val = Through32(cond,(data & 0xff)); 
    uint32_t final = icmd6_val | icmd2_val | icmd3_val | icmd1_4_7_val;
    uint32_t result1 = Through32(type ^ 1,final);
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


void fetch_data(WOLF_CPU* cpu) {
    WCPUFetchData result;
    PWOLF_CPU_ECALL_CONTROLLER* ecall_ctrler = &cpu->ecall_controller;
    PWOLF_CPU_MMU_CONTROLLER* ctrler = &(cpu->mmu);
    MMU_STATUS mmu_res = (*ctrler)->rd_mmu(ctrler,cpu->pc,0b1111);
    if(mmu_res.stat != 0) {
        cpu->ecall_controller->ecaller(ecall_ctrler,ECALL_MACHINE_PROBLEM,MMU_CONVERT_TO_EREASON(mmu_res.stat));
        result.noexception = 0;
        goto WCPU_FETCH_DATA_END;
    }
    uint32_t fetch = mmu_res.data;
    uint8_t type = fetch >> 31;
    uint8_t icode = (fetch >> 25) & 0b0111111;
    uint32_t data = fetch & 0x01ffffff;

    result.irtype = fetch & 0x7fffffff;
    result.icode = icode;
    result.reg1 = getLRegisterA(type,icode, data);
    result.reg2 = getLRegisterB(type,icode,data);
    result.idata = getIData(type, icode, data);
    result.aluExFunc = getExFunc(type,icode,data);
    result.jmpExCond = getExCond(type,icode,data);
    result.ExFlag = getExFlag(type,icode,data);
    
    uint8_t invalid = (icode != ICODE_ALU 
        && icode != ICODE_ECALL 
        && icode != ICODE_ERET_EXFUNC 
        && icode != ICODE_IRET_EXFUNC
        && icode != ICODE_JMP
        && icode != ICODE_LEBR
        && icode != ICODE_LIBR
        && icode != ICODE_LPGR
        && icode != ICODE_LSCR
        && icode != ICODE_MLMR
        && icode != ICODE_MOV
        && icode != ICODE_OCALL
        && icode != ICODE_RET
        && icode != ICODE_RSCR
        && icode != ICODE_LIRE
        && icode != ICODE_LERE); //电路中直接6-64然后对那些未使用的引脚做or就好
    result.noexception = 1;
    if(Through8(result.irtype ^ 1,invalid)) {
        cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,EREASON_FOR_UNSUPPORTED_ICODE); //调用 ecall 函数，代表出现问题
        result.noexception = 0;
    }
WCPU_FETCH_DATA_END:
    cpu->if_data_reg = result;
} 