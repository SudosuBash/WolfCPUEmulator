#include <cpu.h>

void access_check(WOLF_CPU* cpu) {
    WCPUFetchData data = cpu->if_data_reg;
    uint8_t icode = data.icode;
    uint8_t cond = (
        icode == ICODE_LEBR ||
        icode == ICODE_LIBR ||
        icode == ICODE_LPGR ||
        icode == ICODE_LSCR
    );
    if(cond && !IS_IN_KERN_MODE(cpu)) {
        cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,EREASON_FOR_INSTRUCTION_ACCESS_DENIED);
        data.noexception = 0;
    }
}