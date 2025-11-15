#include <cpu.h>

void ecall_proc(WOLF_CPU* cpu) {
    WCPUExecuteResult result = cpu->ex_data_reg;
    if(!result.noexception) return;
    switch (cpu->wb_result_reg.icode)
    {
    case ICODE_ECALL:
        cpu->ecall_controller->ecaller(&cpu->ecall_controller,cpu->wb_result_reg.valC,EREASON_FOR_SOFTWARE_CALLING);
        break;
    case ICODE_RET:
        switch (cpu->wb_result_reg.ExFunc)
        {
        case ICODE_ERET_EXFUNC:
            cpu->ecall_controller->eret_caller(&cpu->ecall_controller);
            break;
        case ICODE_IRET_EXFUNC:
            cpu->ecall_controller->iret_caller(&cpu->ecall_controller);
            break;
        }
    default:
        break;
    }
    result.noexception = 0;
}