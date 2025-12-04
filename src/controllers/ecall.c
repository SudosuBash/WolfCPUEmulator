#include <controllers/ecall.h>
#include <global.h>
#include <cpu.h>
#include <mmio_devices/irq_controller.h>
#include <pthread.h>
#include <stdlib.h>
#include <tools/endian.h>
//同步时序
void ecall(PWOLF_CPU_ECALL_CONTROLLER *ctrl, uint8_t ecode,uint8_t reason,uint32_t earg) {
    WOLF_CPU* cpu = get_parent_struct(ctrl, WOLF_CPU, ecall_controller);
    
    uint32_t base_addr = cpu->ecall_regs.mpc + ecode * ECALL_SINGLE_ITEM_LENGTH;
    cpu->ecall_regs.mep = cpu->pc;
    cpu->ecall_regs.mreason = reason;
    cpu->ecall_regs.mprevmode = (IS_IN_KERN_MODE(cpu) << KU_STORE_BITS) | ((cpu->spe_regs.bcr & BCR_IRQ_DISALLOW_MASK) << IRQ_STORE_BITS);
    cpu->spe_regs.bcr |= (BCR_IRQ_DISALLOW_MASK | (KERN_MODE <<(BCR_KERN_MODE_MASK))); //ecall权限必须是内核模式
    //异常模式也禁用中断吧
    cpu->pc = base_addr;
    cpu->ecall_regs.eargs = earg;
}


//异步时序
void irq_call(PWOLF_CPU_ECALL_CONTROLLER* ctrl) {
    WOLF_CPU* cpu = get_parent_struct(ctrl, WOLF_CPU, ecall_controller);
    if(cpu->spe_regs.bcr & BCR_IRQ_DISALLOW_MASK) return;
    if(!(*ctrl)->external_irq) return;

    BUS_SEND_DATA data = {
        .be = 0b0001,
        .data = {IRQ_CMD_PROCESSING,0,0,0}
    };
    cpu->bus->send_data(&cpu->bus, BUS_IRQ_CONTROLLER_DEVICE_BASE_ADDR + IRQ_CONTROLLER_DEVICE_FUNC_REG_ADDR, data);
    BUS_SEND_DATA recv_val = cpu->bus->recv_data(&cpu->bus,BUS_IRQ_CONTROLLER_DEVICE_BASE_ADDR + IRQ_CONTROLLER_DEVICE_OP_IRQNUM, data);
    uint8_t irqcode = GET_INT_FROM_4_BYTES_L(recv_val.data) & 0xff; 
    
    uint32_t base_addr = cpu->irq_regs.mpc + irqcode * ECALL_SINGLE_ITEM_LENGTH;
    cpu->irq_regs.mep = cpu->pc;
    cpu->irq_regs.mprevmode = IS_IN_KERN_MODE(cpu);
    uint8_t newMode = (cpu->irq_regs.mmode >> irqcode) & 1;
    cpu->pc = base_addr;
    cpu->spe_regs.bcr |= (BCR_IRQ_DISALLOW_MASK | (newMode << (BCR_KERN_MODE_MASK)));
    (*ctrl)->external_irq = 0;
}

void eret(PWOLF_CPU_ECALL_CONTROLLER* ctrl) {
    WOLF_CPU* cpu = get_parent_struct(ctrl, WOLF_CPU, ecall_controller);
    cpu->spe_regs.bcr |= (GET_DATA_BIT_VALUE(cpu->ecall_regs.mprevmode,KU_STORE_BITS) << (BCR_KERN_MODE_MASK)) | (GET_DATA_BIT_VALUE(cpu->ecall_regs.mprevmode,IRQ_STORE_BITS) << (BCR_IRQ_DISALLOW_MASK_BIT)); //恢复特权级和中断使能
    //日后会新增4条特权指令设用来设置ecall的mep mprevmode,用来软件保存状态
    cpu->pc = cpu->ecall_regs.mpc + 4; //跳过当前指令
    cpu->ecall_regs.mep = 0;
    cpu->ecall_regs.mreason = 0;
}
void iret(PWOLF_CPU_ECALL_CONTROLLER* ctrl) {
    WOLF_CPU* cpu = get_parent_struct(ctrl, WOLF_CPU, ecall_controller);
    cpu->spe_regs.bcr &= (0b11111111 ^ BCR_IRQ_DISALLOW_MASK);
    //开NI位的话不允许调用中断
    //没开NI位的话允许调用中断，但是进入中断处理之前绝对是没开中断的，所以处理程序结束后直接把状态恢复为IR就好
    //怕自己忘了写一个注释起
    cpu->spe_regs.bcr &= cpu->irq_regs.mprevmode << (BCR_KERN_MODE_MASK); //恢复特权级
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