#include <cpu.h>

uint32_t getRegVal(WOLF_CPU* cpu,uint8_t lreg) {
    if(lreg != 0 && lreg < MAX_GEN_REGISTER_COUNT) {
        return cpu->gen_regs.r[lreg-1];
    }
    switch(lreg) {
        case CPU_REG_SPE_PGBASE:
            return cpu->spe_regs.pg_mode_base_addr_reg;
        case CPU_REG_ECALL_ECBASE:
            return cpu->ecall_regs.mpc;
        case CPU_REG_ECALL_ICBASE:
            return cpu->irq_regs.mpc;
        case CPU_REG_SPE_BCR:
            return cpu->spe_regs.bcr;
        case CPU_REG_SPE_FLAGS:
            return cpu->spe_regs.flags;
    }
    
}

uint8_t regValid(uint8_t reg1) {
    return !((reg1 == 15 || reg1 == 14) ||
        (reg1 > CPU_REG_ECALL_ICBASE));
}

uint32_t getiData(uint8_t icode,uint32_t idata) {
    uint8_t code_valid = (icode == ICODE_MOV ||
        icode == ICODE_JMP_I ||
        icode == ICODE_OCALL);
    uint32_t idata1 = Through32(code_valid,(idata & 0xff) << 8 | (idata >> 8));
    uint8_t code_valid2 = (icode == ICODE_ALU);
    uint32_t idata2 = Through32(code_valid2,(idata & 0xff) << 4 | (idata >> 8));
    uint8_t code_valid3 = (icode == ICODE_JMP_II);
    uint32_t idata3 = Through32(code_valid3,(idata & 0xff) << 12 | (idata & 0xff00) >> 4 | (idata & 0xf0000) >> 16);
    return Through32(code_valid,idata1) |
        Through32(code_valid2,idata2) |
        Through32(code_valid3,idata3) |
        Through32(!code_valid && !code_valid2 && !code_valid3,idata);
}

WCPUDecodedData decode(WOLF_CPU* cpu) {
    WCPUFetchData data = cpu->if_data_reg;

    WCPUDecodedData res = {0};
    if(!data.noexception) goto CPU_DECODE_END_STATUS;
    uint8_t reg1 = data.reg1;
    uint8_t reg2 = data.reg2;
    
    uint32_t val1 = getRegVal(cpu,reg1);
    uint32_t val2 = getRegVal(cpu,reg2);
    res.ExCond = data.jmpExCond;
    res.destRegs = reg1;
    res.icode = data.icode;
    res.ExFunc = data.aluExFunc;
    res.valA = val1;
    res.valB = val2;
    res.valC = getiData(data.icode,data.idata);
    res.ExFlag = data.ExFlag;
    res.noexception = data.noexception; //上传
    if(!regValid(reg1) || !regValid(reg2)) {
        cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,EREASON_FOR_UNRECOGNIZED_REG);
        res.noexception = 0;    
    }
CPU_DECODE_END_STATUS:
    cpu->id_data_reg = res;
    return res;
}