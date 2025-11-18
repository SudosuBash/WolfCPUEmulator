#ifndef __WOLF_CPU_ECALL
#define __WOLF_CPU_ECALL

#define ECALL_SINGLE_ITEM_LENGTH 16


#define ECALL_MACHINE_PROBLEM 1

#define IREASON_FOR_EXTERNAL_IRQ 0x8

#define EREASON_FOR_UNRECOGNIZED_REG 0x0
#define EREASON_FOR_UNSUPPORTED_ALU_FUNC 0x1
#define EREASON_FOR_UNSUPPORTED_ICODE 0x2
#define EREASON_FOR_INSTRUCTION_ACCESS_DENIED 0x3
#define EREASON_FOR_SOFTWARE_CALLING 0x4
#define EREASON_FOR_RAM_OUT_OF_RANGE 0x9
#define EREASON_FOR_BCR_RAM_ERR_ALIGN 0xA 
#define EREASON_FOR_BCR_RAM_ERR_ACCESS_DENIED 0xB
#define EREASON_FOR_BCR_RAM_OUT_OF_RANGE 0xC

#include <stdint.h>
typedef struct WOLF_CPU_ECALL_CONTROLLER WOLF_CPU_ECALL_CONTROLLER,*PWOLF_CPU_ECALL_CONTROLLER;

typedef void (*ecaller)(PWOLF_CPU_ECALL_CONTROLLER* ctrl,uint8_t ecode, uint8_t reason);
typedef void (*irq_caller)(PWOLF_CPU_ECALL_CONTROLLER* ctrl, uint8_t reason);
typedef void (*eret_caller)(PWOLF_CPU_ECALL_CONTROLLER* ctrl);
typedef void (*iret_caller)(PWOLF_CPU_ECALL_CONTROLLER* ctrl);

struct WOLF_CPU_ECALL_CONTROLLER { 
    
    ecaller ecaller;
    irq_caller irq_caller;
    eret_caller eret_caller;
    iret_caller iret_caller;
};

WOLF_CPU_ECALL_CONTROLLER* init_ecall();
void free_ecall(WOLF_CPU_ECALL_CONTROLLER** ecall);
#endif