#include <controllers/ecall.h>
#include <global.h>
#include <cpu.h>
#include <mmio_devices/irq_controller.h>
#include <pthread.h>
#include <stdlib.h>

//同步时序
void ecall(PWOLF_CPU_ECALL_CONTROLLER *ctrl, uint8_t ecode,uint8_t reason) {
    WOLF_CPU* cpu = get_parent_struct(ctrl, WOLF_CPU, ecall_controller);

    uint32_t base_addr = cpu->ecall_regs.mpc + ecode * ECALL_SINGLE_ITEM_LENGTH;
    cpu->ecall_regs.mep = cpu->pc;
    cpu->ecall_regs.mreason = reason;
    cpu->ecall_regs.mmode = IS_IN_KERN_MODE(cpu);
    uint8_t newMode = (cpu->ecall_regs.memode >> ecode) & 1;
    cpu->spe_regs.bcr |= (newMode << (BCR_KERN_MODE_MASK));
    cpu->pc = base_addr;
}


//异步时序
void irq_call(PWOLF_CPU_ECALL_CONTROLLER* ctrl) {
    WOLF_CPU* cpu = get_parent_struct(ctrl, WOLF_CPU, ecall_controller);
    if(cpu->spe_regs.bcr & BCR_IRQ_DISALLOW_MASK) return;
    if(!(*ctrl)->external_irq) return;

    BUS_SEND_DATA data = {
        .be = 0b0001,
        .data = IRQ_CMD_PROCESS_OK
    };
    cpu->bus->send_data(&cpu->bus, BUS_IRQ_CONTROLLER_DEVICE_BASE_ADDR + IRQ_CONTROLLER_DEVICE_FUNC_REG_ADDR, data);
    BUS_SEND_DATA recv_val = cpu->bus->recv_data(&cpu->bus,BUS_IRQ_CONTROLLER_DEVICE_BASE_ADDR + IRQ_CONTROLLER_DEVICE_OP_IRQNUM, data);
    uint8_t irqcode = recv_val.data & 0xff; 
    
    uint32_t base_addr = cpu->irq_regs.mpc + irqcode * ECALL_SINGLE_ITEM_LENGTH;
    cpu->irq_regs.mep = cpu->pc;
    cpu->irq_regs.mmode = IS_IN_KERN_MODE(cpu);
    uint8_t newMode = (cpu->irq_regs.mimode >> irqcode) & 1;
    cpu->pc = base_addr;
    cpu->spe_regs.bcr |= (BCR_IRQ_DISALLOW_MASK | (newMode << (BCR_KERN_MODE_MASK)));
    (*ctrl)->external_irq = 0;
}

void eret(PWOLF_CPU_ECALL_CONTROLLER* ctrl) {
    WOLF_CPU* cpu = get_parent_struct(ctrl, WOLF_CPU, ecall_controller);
    cpu->spe_regs.bcr &= cpu->ecall_regs.mmode << (BCR_KERN_MODE_MASK); //恢复特权级
    cpu->pc = cpu->ecall_regs.mpc + 4; //跳过当前指令
    cpu->ecall_regs.mep = 0;
    cpu->ecall_regs.mreason = 0;
} 
void iret(PWOLF_CPU_ECALL_CONTROLLER* ctrl) {
    WOLF_CPU* cpu = get_parent_struct(ctrl, WOLF_CPU, ecall_controller);
    cpu->spe_regs.bcr &= (0b11111111 ^ BCR_IRQ_DISALLOW_MASK);
    cpu->spe_regs.bcr &= cpu->irq_regs.mmode << (BCR_KERN_MODE_MASK); //恢复特权级
    cpu->pc = cpu->irq_regs.mpc;
    cpu->ecall_regs.mep = 0;
    cpu->ecall_regs.mreason = 0;
} 

WOLF_CPU_ECALL_CONTROLLER* init_ecall() {
    WOLF_CPU_ECALL_CONTROLLER* ctrl = (WOLF_CPU_ECALL_CONTROLLER*)calloc(1,sizeof(WOLF_CPU_ECALL_CONTROLLER));
    ctrl->ecaller = ecall;
    ctrl->irq_caller = irq_call;
    ctrl->eret_caller = eret;
    ctrl->iret_caller = iret;
    return ctrl;
}

void free_ecall(WOLF_CPU_ECALL_CONTROLLER** ecall) {
    if(*ecall != NULL) {
        free(*ecall);
        *ecall = NULL;
    }
}