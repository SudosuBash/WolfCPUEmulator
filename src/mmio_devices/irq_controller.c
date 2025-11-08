#include <mmio_devices/irq_controller.h>
#include <mmio_devices/device_tools.h>
#include <controllers/ecall.h>
#include <global.h>
#include <string.h>
#include <cpu.h>
#include <pthread.h>
#include <controllers/ecall.h>

#define GET_64_CLEAR_FLAG(num) (0xffffffffffffffff ^ (1 << (num)))
#define GET_64_SET_FLAG(num) (1 << (num))

static void write_reg(PWOLF_CPU_BUS_DEVICE* pdevice,uint8_t addr,BUS_SEND_DATA data) {
    WOLF_CPU_BUS_DEVICE* device = *pdevice;
    WOLF_IRQ_CONTROLLER* dev = get_parent_struct(pdevice,WOLF_IRQ_CONTROLLER, bus_device);
    pthread_mutex_lock(&(dev->device_rwlock));
    uint8_t stat = write_reg_general(device->bus_controller,addr,device->base_address,IRQ_CONTROLLER_DEVICE_REGS,dev->regs);
    if(stat != STAT_SUCCESS) {
        pthread_mutex_unlock(&(dev->device_rwlock));
        return;
    }
    dev->bus_device->device_base_status = 0;
    pthread_mutex_unlock(&(dev->device_rwlock));
}

static void read_reg(PWOLF_CPU_BUS_DEVICE* pdevice,uint8_t addr,uint8_t be) {
    WOLF_CPU_BUS_DEVICE* device = *pdevice;
    WOLF_IRQ_CONTROLLER* dev = get_parent_struct(pdevice,WOLF_IRQ_CONTROLLER, bus_device);
    pthread_mutex_lock(&(dev->device_rwlock));

    uint32_t value;
    uint8_t stat = read_reg_general(device->bus_controller,addr,device->base_address,IRQ_CONTROLLER_DEVICE_REGS,dev->regs);
    if(stat != STAT_SUCCESS) {
        pthread_mutex_unlock(&(dev->device_rwlock));
        return; //dev->bus_device->device_base_status为1的时候，此字段被忽略
    }
    pthread_mutex_unlock(&(dev->device_rwlock));
}

void irq_trigger(WOLF_IRQ_CONTROLLER* controller, WOLF_CPU_BUS_DEVICE* device) {
    pthread_mutex_lock(&(controller->device_rwlock));
    //加速处理，默认一切参数合法，否则太慢了
    uint8_t device_irq_num = device->irq_number;
    if(! controller->registered_interrupts[device->irq_number].enabled) {
        pthread_mutex_unlock(&(controller->device_rwlock)); 
        return; //禁止中断直接返回
    }
    controller->registered_interrupts[device->irq_number].status = IRQ_STATUS_SUSPEND; //改为挂起
    controller->int_valid_flag |= GET_64_SET_FLAG(device->irq_number);
    pthread_mutex_unlock(&(controller->device_rwlock));
}

void set_irq_disable(WOLF_IRQ_CONTROLLER* ctrl,uint8_t irq_num) {
    ctrl->registered_interrupts[irq_num].enabled = 0;
    ctrl->int_valid_flag &= GET_64_CLEAR_FLAG(irq_num);
    ctrl->regs[IRQ_CONTROLLER_DEVICE_STAT_REG_ADDR] = 0;
}

void set_irq_enable(WOLF_IRQ_CONTROLLER* ctrl,uint8_t irq_num) {
    ctrl->registered_interrupts[irq_num].enabled = 1;
    ctrl->int_valid_flag |= GET_64_SET_FLAG(ctrl->registered_interrupts[irq_num].enabled &&
    						!ctrl->registered_interrupts[irq_num].ignored &&
                            ctrl->registered_interrupts[irq_num].status == IRQ_STATUS_SUSPEND);
                            //挂起的时候才是有效的
    ctrl->regs[IRQ_CONTROLLER_DEVICE_STAT_REG_ADDR] = 0;
}

void set_irq_ignore(WOLF_IRQ_CONTROLLER* ctrl,uint8_t irq_num) {
    ctrl->registered_interrupts[irq_num].ignored = IRQ_IGNORED;
    ctrl->int_valid_flag &= GET_64_CLEAR_FLAG(irq_num);
    ctrl->regs[IRQ_CONTROLLER_DEVICE_STAT_REG_ADDR] = 0;
}

void clear_irq_ignore(WOLF_IRQ_CONTROLLER* ctrl,uint8_t irq_num) {
    ctrl->registered_interrupts[irq_num].ignored = IRQ_NOT_IGNORED;
    ctrl->int_valid_flag |= GET_64_SET_FLAG(ctrl->registered_interrupts[irq_num].enabled &&
    						!ctrl->registered_interrupts[irq_num].ignored  &&
                            ctrl->registered_interrupts[irq_num].status == IRQ_STATUS_SUSPEND);
    ctrl->regs[IRQ_CONTROLLER_DEVICE_STAT_REG_ADDR] = 0;
}

void set_interrupt_ok(WOLF_IRQ_CONTROLLER* ctrl,uint8_t irq_num) {
    ctrl->registered_interrupts[irq_num].status = IRQ_STATUS_OKAY;
    ctrl->regs[IRQ_CONTROLLER_DEVICE_STAT_REG_ADDR] = 0;
    ctrl->int_valid_flag &= GET_64_CLEAR_FLAG(irq_num);
}

void set_priority(WOLF_IRQ_CONTROLLER* ctrl,uint8_t irq_num,uint8_t prio) {
    if(irq_num >= IRQ_MAX_PRIO) {
        ctrl->bus_device->device_base_status = 1;
        ctrl->regs[IRQ_CONTROLLER_DEVICE_STAT_REG_ADDR] = 3;
        return;
    }
    ctrl->regs[IRQ_CONTROLLER_DEVICE_STAT_REG_ADDR]=0;
    ctrl->registered_interrupts[irq_num].status = IRQ_STATUS_OKAY;
}

void device_start(PWOLF_CPU_BUS_DEVICE* pdevice) {
    WOLF_CPU_BUS_DEVICE* device = *pdevice;
    WOLF_IRQ_CONTROLLER* controller = get_parent_struct(pdevice,WOLF_IRQ_CONTROLLER,bus_device);
    WOLF_CPU_BUS_CONTROLLER* bus_controller = device->bus_controller;
    
    uint32_t address = bus_controller->addr;
    if(bus_controller->addr < device->base_address + device->need_space && bus_controller->addr >= device->base_address) {
        if(bus_controller->data_cmd_collection.read_write == BUS_RW_READ) 
            read_reg(pdevice,address,bus_controller->data_cmd_collection.be);
        else if(bus_controller->data_cmd_collection.read_write == BUS_RW_WRITE) 
            write_reg(pdevice,address,bus_controller->data_cmd_collection);
        return; //下一次循环，开始处理命令
    }
    //这个模拟不同时序，异步执行
    WOLF_CPU* cpu = get_parent_struct(&device->bus_controller,WOLF_CPU,bus);

    pthread_mutex_lock(&(controller->device_rwlock));
    uint8_t cmd = controller->regs[IRQ_CONTROLLER_DEVICE_FUNC_REG_ADDR];
    uint8_t irq_num = controller->regs[IRQ_CONTROLLER_DEVICE_OP_IRQNUM];
    // MUX 的加速版
    switch(cmd) { //还是tmd加速
        case IRQ_CMD_SET_DISABLE:
            set_irq_disable(controller,irq_num);
            break;
        case IRQ_CMD_SET_ENABLE:
            set_irq_enable(controller,irq_num);
            break;
        case IRQ_CMD_SET_IGNORE:
            set_irq_ignore(controller,irq_num);
            break;
        case IRQ_CMD_CLEAR_IGNORE:
            clear_irq_ignore(controller,irq_num);
            break;
        case IRQ_CMD_PROCESS_OK:
            set_interrupt_ok(controller,irq_num);
            break;
        case IRQ_CMD_SET_PRIORITY:
            uint8_t prio = controller->regs[IRQ_CONTROLLER_DEVICE_PRIO_SET];
            set_priority(controller,irq_num,prio);
            break;
        case IRQ_CMD_PROCESSING:
        	int16_t min_prio = IRQ_MAX_PRIO,min_index = -1;
    		for(uint8_t i=0;i<IRQ_INTERRUPTS_SUM;i++) { //本来应该是分组求和并行，但是这样会导致代码量爆炸，所以最终我还是用for循环吧
        		if(controller->registered_interrupts[i].prio < min_prio && 
            		controller->registered_interrupts[i].enabled == 1 &&
            		controller->registered_interrupts[i].status == IRQ_STATUS_SUSPEND &&
            		controller->registered_interrupts[i].ignored == IRQ_NOT_IGNORED) {
                		min_index = i;
                		min_prio = controller->registered_interrupts[i].prio;
        		}
            } //开始仲裁
            if(min_index != -1) { //不用按位的原因还是加速
        		controller->registered_interrupts[min_index].status = IRQ_STATUS_PROCESSING;
        		controller->regs[IRQ_CONTROLLER_DEVICE_SUBDEVICE_NUM_REG] =  controller->registered_interrupts[min_index].irq_num;
                controller->regs[IRQ_CONTROLLER_DEVICE_STAT_REG_ADDR] = 0;
            } else {
                controller->regs[IRQ_CONTROLLER_DEVICE_STAT_REG_ADDR] = 1;
            }
            break;
    }
    if(controller->int_valid_flag) {
        cpu->ecall_controller->irq_caller(&cpu->ecall_controller,IREASON_FOR_EXTERNAL_IRQ);
    }
    controller->regs[IRQ_CONTROLLER_DEVICE_FUNC_REG_ADDR] = 0;
    pthread_mutex_unlock(&(controller->device_rwlock));
}

WOLF_IRQ_CONTROLLER* init_irq_controller(WOLF_CPU_BUS_CONTROLLER* controller) {
    WOLF_CPU_BUS_DEVICE* bus_device = (WOLF_CPU_BUS_DEVICE*)calloc(1,sizeof(WOLF_CPU_BUS_DEVICE));
    WOLF_IRQ_CONTROLLER* irq_controller = (WOLF_IRQ_CONTROLLER*) calloc(1,sizeof(WOLF_IRQ_CONTROLLER));
    if (bus_device == NULL || irq_controller == NULL) return NULL;
    memset(irq_controller,0,sizeof(WOLF_IRQ_CONTROLLER));
    memset(bus_device,0,sizeof(WOLF_CPU_BUS_DEVICE));

    bus_device->bus_controller = controller;
    irq_controller->bus_device = bus_device;
    irq_controller->trigger_fn = irq_trigger;
    
    strncpy(bus_device->name,device_name,DEVICE_NAME_STR_MAX);
    strncpy(bus_device->vendor,vendor_name,DEVICE_VENDOR_STR_MAX);
    bus_device->vendor_id = device_id;
    bus_device->need_space = need_space;
    bus_device->base_address = BUS_IRQ_CONTROLLER_DEVICE_BASE_ADDR;
    bus_device->wr_reg_func = write_reg;
    bus_device->rd_reg_func = read_reg;
    bus_device->start_func = device_start;

    for(int i = 0; i < IRQ_INTERRUPTS_SUM; i++) {
        irq_controller->registered_interrupts[i].irq_num = i;
        irq_controller->registered_interrupts[i].enabled = 0; 
        irq_controller->registered_interrupts[i].status = IRQ_STATUS_OKAY;
        irq_controller->registered_interrupts[i].ignored = IRQ_NOT_IGNORED;
        irq_controller->registered_interrupts[i].prio = IRQ_MAX_PRIO; 
    }
    pthread_mutex_init(&(irq_controller->device_rwlock),NULL);
    return irq_controller;
}