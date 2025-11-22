#include <cpu.h>

uint32_t getiData(uint8_t icode,uint32_t idata,uint32_t excond) {
    uint8_t code_valid = (icode == ICODE_MOV ||
        (icode == ICODE_JMP) ||
        icode == ICODE_OCALL);
    uint32_t idata1 = Through32(code_valid,(idata & 0xff) << 8 | (idata >> 8));
    uint8_t code_valid2 = (icode == ICODE_ALU);
    uint32_t idata2 = Through32(code_valid2,(idata & 0xff) << 4 | (idata >> 8));
    
    return Through32(code_valid,idata1) |
        Through32(code_valid2,idata2) |
        Through32(!code_valid && !code_valid2,idata);
}

uint8_t getDestReg(uint8_t icode,uint8_t reg1,uint8_t reg2) {
    uint8_t cond = (icode == ICODE_OCALL || icode == ICODE_PUSH);
    uint8_t cond2 = (icode == ICODE_JMP); //不需要写入寄存器的指令
    return Through8(cond,reg2) |
        Through8(cond2,0) |
        Through8(!cond && !cond2,reg1);
}
void decode(WOLF_CPU* cpu) {

    if(!cpu->temp_data_reg.noexception) goto CPU_DECODE_END_STATUS;
    uint8_t reg1 = cpu->temp_data_reg.reg1;
    uint8_t reg2 = cpu->temp_data_reg.reg2;
    uint8_t destReg = getDestReg(cpu->temp_data_reg.icode,reg1,reg2);
    if(destReg == CPU_REG_PC) {
        cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,EREASON_FOR_UNSUPPORTED_ICODE,cpu->temp_data_reg.instruction);
        cpu->temp_data_reg.noexception = 0;
        goto CPU_DECODE_END_STATUS;
    }//限制:destReg不能是PC
    uint8_t destReg2 = Through8(cpu->temp_data_reg.icode == ICODE_POP,reg2);
    uint32_t val1 = getRegVal(cpu,reg1);
    uint32_t val2 = getRegVal(cpu,reg2);
    cpu->temp_data_reg.destReg = destReg;
    cpu->temp_data_reg.destReg2 = destReg2;
    cpu->temp_data_reg.valA = val1;
    cpu->temp_data_reg.valB = val2;
    cpu->temp_data_reg.valC = getiData(cpu->temp_data_reg.icode,cpu->temp_data_reg.valC,cpu->temp_data_reg.ExCond);
CPU_DECODE_END_STATUS:
}