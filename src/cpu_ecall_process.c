#include <cpu.h>

void ecall_proc(WOLF_CPU* cpu) {
    if(cpu->temp_data_reg.noexception) return;
    switch (cpu->temp_data_reg.icode)
    {
    case ICODE_ECALL:
        cpu->ecall_controller->ecaller(&cpu->ecall_controller,cpu->temp_data_reg.valC,EREASON_FOR_SOFTWARE_CALLING,0);
        break;
    case ICODE_RET:
        switch (cpu->temp_data_reg.ExFunc)
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
    cpu->temp_data_reg.noexception = 0;
}