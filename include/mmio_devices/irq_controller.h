#ifndef __WOLF_IRQ_CONTROLLER
#define __WOLF_IRQ_CONTROLLER
/**
 * 简化版中断控制器
*/
#include <controllers/bus.h>
#include <logics/logic_alg.h>
#include <stdint.h>
#include <pthread.h>
#include <cpu.h>

#define IRQ_CONTROLLER_DEVICE_FUNC_REG_ADDR 0x0
#define IRQ_CONTROLLER_DEVICE_STAT_REG_ADDR 0x1
#define IRQ_CONTROLLER_DEVICE_OP_IRQNUM 0x2
#define IRQ_CONTROLLER_DEVICE_SUBDEVICE_NUM_REG 0x3
#define IRQ_CONTROLLER_DEVICE_PRIO_SET 0x4

#define IRQ_CMD_SET_IGNORE 0x1
#define IRQ_CMD_CLEAR_IGNORE 0x2
#define IRQ_CMD_SET_DISABLE 0x3
#define IRQ_CMD_SET_ENABLE 0x4
#define IRQ_CMD_PROCESS_OK 0x5
#define IRQ_CMD_SET_PRIORITY 0x6
#define IRQ_CMD_PROCESSING 0x7

#define IRQ_STATUS_OKAY 0x0
#define IRQ_STATUS_SUSPEND 0x1
#define IRQ_STATUS_PROCESSING 0x2

#define IRQ_CONTROLLER_DEVICE_REGS 8
#define BUS_IRQ_CONTROLLER_DEVICE_BASE_ADDR 0xFFFF000A
#define IRQ_MAX_PRIO 3
#define IRQ_INTERRUPTS_SUM 64
#define IRQ_INT_IN_CPU 0x1

#define IRQ_IGNORED 0x1
#define IRQ_NOT_IGNORED 0

typedef struct {
    uint32_t irq_num;
    uint32_t addr;
    uint32_t prio;
    uint8_t enabled:1;
    uint8_t ignored:1;
    uint8_t status:2;
} INTERRUPT_DESC;

typedef struct WOLF_IRQ_CONTROLLER WOLF_IRQ_CONTROLLER,*PWOLF_IRQ_CONTROLLER;
typedef void (*irq_trigger_fn)(WOLF_IRQ_CONTROLLER* irq_controller, WOLF_CPU_BUS_DEVICE* device);


struct WOLF_IRQ_CONTROLLER {
    WOLF_CPU_BUS_DEVICE* bus_device;
    uint8_t regs[IRQ_CONTROLLER_DEVICE_REGS];
    pthread_mutex_t device_rwlock;
    
    irq_trigger_fn trigger_fn;    //预留一个硬件接口,对中断作请求
    INTERRUPT_DESC registered_interrupts[IRQ_INTERRUPTS_SUM]; //64个
    uint64_t int_valid_flag;
    
    uint8_t external_irq_req;
};
PWOLF_CPU_BUS_DEVICE* init_irq_controller(WOLF_CPU_BUS_CONTROLLER* controller,WOLF_CPU* cpu_instance);
void destroy_irq_device(PWOLF_IRQ_CONTROLLER* pirq_device);
#endif