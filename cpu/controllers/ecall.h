#ifndef __WOLF_CPU_ECALL
#define __WOLF_CPU_ECALL

#define ECALL_SINGLE_ITEM_LENGTH 16
#define IRQ_DISALLOW_MASK 8

#define REASON_FOR_EXTERNAL_IRQ 0x8
#include <stdint.h>
typedef struct WOLF_CPU_ECALL_CONTROLLER WOLF_CPU_ECALL_CONTROLLER;

typedef void (*ecaller)(WOLF_CPU_ECALL_CONTROLLER* ctrl,uint8_t ecode, uint8_t reason);
typedef void (*irq_caller)(WOLF_CPU_ECALL_CONTROLLER* ctrl,uint8_t irqcode, uint8_t reason);
typedef void (*eret_caller)(WOLF_CPU_ECALL_CONTROLLER* ctrl);
typedef void (*iret_caller)(WOLF_CPU_ECALL_CONTROLLER* ctrl);

struct WOLF_CPU_ECALL_CONTROLLER { 
    
    ecaller ecaller;
    irq_caller irq_caller;
    eret_caller eret_caller;
    iret_caller iret_caller;
};
void ecall(WOLF_CPU_ECALL_CONTROLLER *ctrl, uint8_t ecode,uint8_t reason);
void irq_call(WOLF_CPU_ECALL_CONTROLLER* ctrl,uint8_t irqcode, uint8_t reason);
void eret(WOLF_CPU_ECALL_CONTROLLER* ctrl);
void iret(WOLF_CPU_ECALL_CONTROLLER* ctrl);
#endif