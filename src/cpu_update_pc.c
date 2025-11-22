#include <cpu.h>

void update_PC(WOLF_CPU* cpu) {
    if(cpu->temp_data_reg.noexception == 0) return;
    uint8_t icode = cpu->temp_data_reg.icode;
    uint8_t cond = (
        icode == ICODE_OCALL || 
        icode == ICODE_RET);
    uint32_t excond = cpu->temp_data_reg.ExCond & 0b1111;
    uint8_t val1 = 0;
        switch(cpu->temp_data_reg.ExCond) {
            case JMP_EXCOND_JMP:
                val1 = 1;
                break;
            case JMP_EXCOND_EQ:
                val1 = GET_SCR_ZF_FLAG(cpu->spe_regs.scr);
                break;
            case JMP_EXCOND_G: //大于，greater
                val1 = GET_SCR_SF_FLAG(cpu->spe_regs.scr) == GET_SCR_OF_FLAG(cpu->spe_regs.scr) && !GET_SCR_ZF_FLAG(cpu->spe_regs.scr);
                break;
            case JMP_EXCOND_S: //小于，smaller
                val1 = GET_SCR_SF_FLAG(cpu->spe_regs.scr) != GET_SCR_OF_FLAG(cpu->spe_regs.scr);
                break;
            case JMP_EXCOND_A:
                val1 = !GET_SCR_CF_FLAG(cpu->spe_regs.scr) && !GET_SCR_ZF_FLAG(cpu->spe_regs.scr);
                break;
            case JMP_EXCOND_B:
                val1 = GET_SCR_CF_FLAG(cpu->spe_regs.scr);
                break;
            case JMP_EXCOND_NE:
                val1 = !GET_SCR_ZF_FLAG(cpu->spe_regs.scr);
                break;
        }

        uint32_t valc = Through32(icode == ICODE_JMP && val1,cpu->temp_data_reg.valC) |
        Through32(icode == ICODE_JMP && !val1,cpu->temp_data_reg.valP);
        //JMP指令,val1为满足条件
    uint32_t updated_PC = valc |
        Through32(cond,cpu->temp_data_reg.valC) |
        Through32(!cond && icode != ICODE_JMP,cpu->temp_data_reg.valP);
    cpu->pc = updated_PC;
}