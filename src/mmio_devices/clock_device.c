#include <mmio_devices/clock_device.h>
#include <mmio_devices/device_tools.h>
#include <logics/logic_alg.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <controllers/bus.h>
#include <time.h>
#include <cpu.h>

static const char device_name[] = "Wolf Clock Controller";
static const char vendor_name[] = "Wolf Emulator";
static const uint8_t need_space = CLOCK_DEVICE_REGS;
static const uint16_t device_id = 0x1004;

static WOLF_IRQ_CONTROLLER* irq_controller;

static void write_reg(PWOLF_CPU_BUS_DEVICE* pdevice,uint8_t addr,BUS_SEND_DATA data) {
    WOLF_CPU_BUS_DEVICE* device = *pdevice;
    WOLF_MMIO_CLOCK_DEVICE* dev = get_parent_struct(pdevice,WOLF_MMIO_CLOCK_DEVICE, bus_device);
    pthread_rwlock_wrlock(&(dev->device_rwlock));
    uint8_t stat = write_reg_general(device->bus_controller,addr,device->base_address,CLOCK_DEVICE_REGS,dev->regs);
    if(stat == 0) {
        device->bus_controller->data_cmd_collection.status = BUS_STATUS_SUCCESS;
        dev->bus_device->device_base_status = 1;
        pthread_rwlock_unlock(&(dev->device_rwlock));
        return;
    }
    device->bus_controller->data_cmd_collection.status = BUS_STATUS_ERROR;
    dev->bus_device->device_base_status = 0;
    pthread_rwlock_unlock(&(dev->device_rwlock));
}

static void read_reg(PWOLF_CPU_BUS_DEVICE* pdevice,uint8_t addr,uint8_t be) {
    WOLF_CPU_BUS_DEVICE* device = *pdevice;
    WOLF_MMIO_CLOCK_DEVICE* dev = get_parent_struct(pdevice,WOLF_MMIO_CLOCK_DEVICE,bus_device);
    pthread_rwlock_rdlock(&(dev->device_rwlock));
    uint8_t stat = read_reg_general(device->bus_controller,addr,device->base_address,CLOCK_DEVICE_REGS,dev->regs);

    if(stat == 0) {
        device->bus_controller->data_cmd_collection.status = BUS_STATUS_SUCCESS;
        dev->bus_device->device_base_status = 1;
        pthread_rwlock_unlock(&(dev->device_rwlock));
    }
    device->bus_controller->data_cmd_collection.status = BUS_STATUS_ERROR;
    pthread_rwlock_unlock(&(dev->device_rwlock));
}

static void device_start(PWOLF_CPU_BUS_DEVICE* pdevice) {
    WOLF_CPU_BUS_DEVICE* device = *pdevice;
    WOLF_MMIO_CLOCK_DEVICE* dev = get_parent_struct(pdevice,WOLF_MMIO_CLOCK_DEVICE,bus_device);

    PROCESS_DEVICE_REGISTER_WRITING(device);
    
    pthread_rwlock_wrlock(&(dev->device_rwlock));

    if(dev->regs[CLOCK_DEVICE_OPEN_ADDR]) {
        usleep(30000+dev->regs[CLOCK_DEVICE_WAIT_DELTA] * 100); //最简单的时间中断
        irq_controller->trigger_fn(irq_controller,device);
    }
    pthread_rwlock_unlock(&(dev->device_rwlock));
}
//只能调用一次
PWOLF_CPU_BUS_DEVICE* init_clock_device(WOLF_CPU_BUS_CONTROLLER* controller, WOLF_IRQ_CONTROLLER* irq) {
    WOLF_CPU_BUS_DEVICE* bus_device = (WOLF_CPU_BUS_DEVICE*)calloc(1,sizeof(WOLF_CPU_BUS_DEVICE));
    WOLF_MMIO_CLOCK_DEVICE* clock_device = (WOLF_MMIO_CLOCK_DEVICE*)calloc(1,sizeof(WOLF_MMIO_CLOCK_DEVICE));
    irq_controller = irq;
    if(clock_device == NULL || bus_device == NULL)
        return NULL; 

    INIT_BUS_DEVICE(bus_device,device_name,vendor_name,controller,device_id,need_space,device_start,read_reg,write_reg);
    
    clock_device->bus_device = bus_device;
#ifdef _EMU_MMIO_DEBUG
    bus_device->base_address = 0xffff00F0;
#endif
    pthread_rwlock_init(&(clock_device->device_rwlock),NULL);
    pthread_mutex_init(&(clock_device->stdio_lock),NULL);
    return &clock_device->bus_device; //被坑惨了qwq
}
//只能调用一次
void destroy_clock_device(PWOLF_MMIO_CLOCK_DEVICE* pclock_device) { //设备结束运行时候调用的
    WOLF_MMIO_CLOCK_DEVICE* clock_device = *pclock_device;
    pthread_rwlock_destroy(&(clock_device->device_rwlock));
    pthread_mutex_destroy(&(clock_device->stdio_lock));
    if(clock_device != NULL) {
        if (clock_device->bus_device != NULL) {
            free(clock_device->bus_device);
        }
        free(clock_device);
        *pclock_device = NULL;
    }
}
