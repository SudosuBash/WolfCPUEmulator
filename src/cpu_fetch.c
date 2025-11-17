#include <controllers/ecall.h>
#include <cpu.h>

uint8_t regValid(uint8_t reg1) {
    return ! (reg1 > CPU_REG_IRQ_REASON);
}

static inline uint8_t getExCond(uint8_t type,uint8_t icode,uint32_t data) {
    uint8_t val = Through8(icode == ICODE_JMP,(data >> 16) & 0b1111);
    //以上为 I 类指令的解码器
    return Through8(IS_ITYPE(type),val);
}


static inline uint8_t getLRegisterB(uint8_t type,uint8_t icode, uint32_t data) {
    uint8_t icodeB = 
        (icode == ICODE_ECALL);
    uint8_t regB = (data >> 21) & 0xf;
    uint8_t regB2 = CPU_REG_ECALL_ECBASE;
    //Ecall: rB = spe.ecall_base
    uint8_t final_itype_val = Through8(icodeB,regB2);
    //以上为 I 类指令的解码器

    uint8_t icodeA = (
        icode == ICODE_LBCR ||
        icode == ICODE_LPGR ||
        icode == ICODE_LEBR ||
        icode == ICODE_LIBR
    );
    uint8_t regA = (data >> 21) & 0xf;
    uint8_t icodeC = (icode == ICODE_OCALL 
        || icode == ICODE_RET 
        || icode == ICODE_PUSH 
        || icode == ICODE_POP);
    uint8_t regB3 = CPU_REG_RSP;
    uint8_t icodeH = (icode == ICODE_RERE);
    uint8_t regB8 = CPU_REG_ECALL_REASON;
    uint8_t icodeI = (icode == ICODE_RIRE);
    uint8_t regB9 = CPU_REG_IRQ_REASON;

        
    uint8_t icodeJ = (icode == ICODE_ECALL);
    uint8_t regB10 = CPU_REG_PC;
    
    uint8_t icodeK = (icode == ICODE_MOV || icode == ICODE_ALU);
    uint8_t regB11 = (data >> 17) & 0xf;
    uint8_t final_rtype_val = 
        Through8(icodeA,regA) |
        Through8(icodeC,regB3) |
        Through8(icodeH,regB8) |
        Through8(icodeI,regB9) |
        Through8(icodeJ,regB10) |
        Through8(icodeK,regB11);
    //以上为 R 类指令的解码器
    return Through8(IS_ITYPE(type), final_itype_val) |
        Through8(IS_RTYPE(type),final_rtype_val);
}


static inline uint8_t getExFlag(uint8_t type,uint8_t icode,uint32_t data) {
    uint8_t cond = icode == ICODE_MOV; 
    uint8_t movExFlag = (data >> 16) & 0b11111;
    uint8_t val1 = Through8(cond,movExFlag);
        
    cond = (icode == ICODE_ALU || cond == ICODE_MLMR);
    uint8_t val2 = Through8(cond,(data >> 12) & 0b11111);
    uint8_t final_itype_val =val1 | val2;
    //以上为 I 类指令的解码器
    cond = (icode == ICODE_OCALL) || (icode == ICODE_ZWC);
    uint8_t val3 = Through8(cond,(data >> 16) & 0b11111);
    cond = (
        icode == ICODE_MOV
    );
    uint8_t val4 = Through8(cond,(data >> 12) & 0b11111);
    cond = (icode == ICODE_ALU);
    uint8_t val5 = Through8(cond,(data >> 8) & 0b11111);
    uint8_t final_rtype_val = val3 | val4 | val5;
    //以上为 R 类指令的解码器
    return Through8(IS_ITYPE(type), final_itype_val) |
        Through8(IS_RTYPE(type),final_rtype_val);
}

static inline uint8_t getLRegisterA(uint8_t icode,uint32_t data,uint8_t ExFlag,uint8_t ExCond) {
    uint8_t icodeA = 
      (icode == ICODE_MOV) ||
      (icode == ICODE_ALU) || 
      (icode == ICODE_MLMR) ||
      (icode == ICODE_RBCR) ||
      (icode == ICODE_RIRE) ||
      (icode == ICODE_RERE) ||
      (icode == ICODE_ZWC) ||
      (icode == ICODE_PUSH) ||
      (icode == ICODE_POP);
    uint8_t regA = (data >> 21) & 0xf;
    
    uint8_t icodeB = 
        (icode == ICODE_RET);
    uint8_t regB = CPU_REG_RSP;

    uint8_t icodeC = 
        (icode == ICODE_OCALL);
    uint8_t r1_on = EXFLAG_R1_ON(ExFlag);
    uint8_t regC = Through8(r1_on,regA) |
        Through8(!r1_on,CPU_REG_PC);
    //CALL A设定为具体的值，B设定为RSP

    uint8_t icodeD = (icode == ICODE_JMP);
    r1_on = EXCOND_R1_ON(ExCond);
    uint8_t regD = Through8(r1_on,regA) |
         Through8(!r1_on,CPU_REG_PC);
    //JMP A设置为具体的值
        //Call: rB = rsp
    uint8_t icodeE = (icode == ICODE_LBCR);
    uint8_t regA5 = CPU_REG_SPE_BCR;
    //LBCR: rB = SCR
    uint8_t icodeF = (icode == ICODE_LPGR);
    uint8_t regA6 = CPU_REG_SPE_PGBASE;
    //Lpgr: rb = PGBASE
    uint8_t icodeG = (icode == ICODE_LEBR);
    uint8_t regA7 = CPU_REG_ECALL_ECBASE;
    //Lebr: rb = ECBASE
    uint8_t icodeH = (icode == ICODE_LIBR);
    //Libr: rb = ICBASE
    uint8_t regA8 = CPU_REG_ECALL_ICBASE;
    uint8_t icodeI = (icode == ICODE_PUSHF) || (icode == ICODE_POPF);
    uint8_t regA9 = CPU_REG_SPE_SCR;
    uint8_t datares = Through8(icodeA, regA) |
        Through8(icodeB,regB) |
        Through8(icodeC,regC) |
        Through8(icodeD,regD) |
        Through8(icodeE,regA5) |
        Through8(icodeF,regA6) |
        Through8(icodeG,regA7) |
        Through8(icodeH,regA8) |
        Through8(icodeI,regA9);
    return datares;
}

static inline uint32_t getIData(uint8_t type, uint8_t icode,uint32_t data) {
    uint8_t cond = (icode == ICODE_MOV);
    uint32_t icmd1_4_7_val = Through32(cond,(data & 0xffff));
    cond = (icode == ICODE_ALU);
    uint32_t icmd2_val = Through32(cond,(data & 0x0fff));
    cond = (icode == ICODE_MLMR);
    uint32_t icmd3_val = Through32(cond,(data & 0x1f));

    cond = (icode == ICODE_ECALL);
    uint32_t icmd6_val = Through32(cond,(data & 0x3f)); //ECALL 的中断号
    uint32_t final_itype_res = icmd6_val | icmd2_val | icmd3_val | icmd1_4_7_val;
    //以上为 I 类指令的解码器
    cond = (icode == ICODE_JMP || icode == ICODE_OCALL);
    uint32_t icmdb_val = Through32(cond,(data & 0xffff));
    return Through32(IS_ITYPE(type),final_itype_res) |
        Through32(IS_RTYPE(type),icmdb_val);
}

static inline uint8_t getExFunc(uint8_t type,uint8_t icode,uint32_t data) {
    uint8_t cond = (
      icode == ICODE_ALU ||
      icode == ICODE_MLMR 
    );
    uint8_t val = Through8(cond,(data >> 17) & 0b1111);
    uint8_t final_itype_val = val;
    //以上为 I 类指令的解码器
    cond = (icode == ICODE_ALU);
    uint8_t valB1 = Through8(cond,(data >> 13));
    cond = (ICODE_RET == icode);
    uint8_t val2 = Through8(cond,(data >> 21) & 0b1111);
    //以上为 R 类指令的解码器
    return Through8(IS_ITYPE(type),final_itype_val) |
        Through8(IS_RTYPE(type),valB1 | val2);
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
    uint8_t ExFlag = getExFlag(type,icode,data);
    result.aluExFunc = getExFunc(type,icode,data);
    result.jmpExCond = getExCond(type,icode,data);
    result.ExFlag = ExFlag;
    result.irtype = type;
    result.icode = icode;
    result.reg1 = getLRegisterA(icode, data, ExFlag, result.jmpExCond);
    result.reg2 = getLRegisterB(type,icode,data);
    result.idata = getIData(type, icode, data);
    result.valP = cpu->pc + 4;

    result.noexception = 1;
    if(IS_ICODE_INVALID(icode,type)) {
        cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,EREASON_FOR_UNSUPPORTED_ICODE); //调用 ecall 函数，代表出现问题
        result.noexception = 0;
    }
    if(!regValid(result.reg1) || !regValid(result.reg2)) {
        cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,EREASON_FOR_UNRECOGNIZED_REG);
        result.noexception = 0;
    }
WCPU_FETCH_DATA_END:
    cpu->if_data_reg = result;
} 