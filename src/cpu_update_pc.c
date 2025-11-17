#include <cpu.h>

void update_PC(WOLF_CPU* cpu) {
    WCPUWBResult result = cpu->wb_result_reg;
    if(result.noexception == 0) return;
    uint8_t icode = result.icode;
    uint8_t cond = (icode == ICODE_JMP ||
        icode == ICODE_ECALL ||
        icode == ICODE_OCALL ||
        icode == ICODE_RET);
    uint32_t updated_PC = Through32(cond,result.valC) |
        Through32(!cond,result.valP);
    cpu->pc = updated_PC;
}