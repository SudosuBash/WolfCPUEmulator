#include <mmio_devices/stdo_device.h>
#include <mmio_devices/device_tools.h>
#include <logics/logic_alg.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <controllers/bus.h>

static void write_reg(PWOLF_CPU_BUS_DEVICE* pdevice,uint8_t addr,BUS_SEND_DATA data) {
    WOLF_CPU_BUS_DEVICE* device = *pdevice;
    WOLF_MMIO_STDO_DEVICE* dev = get_parent_struct(pdevice,WOLF_MMIO_STDO_DEVICE, bus_device);
    pthread_rwlock_wrlock(&(dev->device_rwlock));
    uint8_t stat = write_reg_general(device->bus_controller,addr,device->base_address,STDO_DEVICE_REGS,dev->regs);
    if(stat == 0) {
        dev->bus_device->device_base_status = 1;
        pthread_rwlock_unlock(&(dev->device_rwlock));
        return;
    }
    dev->bus_device->device_base_status = 0;
    pthread_rwlock_unlock(&(dev->device_rwlock));
}

static void read_reg(PWOLF_CPU_BUS_DEVICE* pdevice,uint8_t addr,uint8_t be) {
    WOLF_CPU_BUS_DEVICE* device = *pdevice;
    WOLF_MMIO_STDO_DEVICE* dev = get_parent_struct(pdevice,WOLF_MMIO_STDO_DEVICE,bus_device);
    pthread_rwlock_rdlock(&(dev->device_rwlock));
    uint8_t stat = read_reg_general(device->bus_controller,addr,device->base_address,STDO_DEVICE_REGS,dev->regs);

    if(stat == 0) {
        dev->bus_device->device_base_status = 1;
        pthread_rwlock_unlock(&(dev->device_rwlock));
    }
    pthread_rwlock_unlock(&(dev->device_rwlock));
}

static void device_output(WOLF_MMIO_STDO_DEVICE* device,uint8_t write_buf) {
    printf("%c",(char)(write_buf));
    device->regs[STDO_DEVICE_STAT_REG] = STDO_STAT_OK;
}

static void device_start(PWOLF_CPU_BUS_DEVICE* pdevice) {
    WOLF_CPU_BUS_DEVICE* device = *pdevice;
    WOLF_MMIO_STDO_DEVICE* dev = get_parent_struct(pdevice,WOLF_MMIO_STDO_DEVICE,bus_device);
    pthread_rwlock_wrlock(&(dev->device_rwlock));
    if(dev->bus_device->device_busy) { //这用if纯粹是为了效率，否则后面的也会执行
        pthread_rwlock_unlock(&(dev->device_rwlock));
        return;
    }


    WOLF_CPU_BUS_CONTROLLER* bus_controller = device->bus_controller;
    uint32_t address = bus_controller->addr;
    if(bus_controller->addr < device->base_address + device->need_space && bus_controller->addr >= device->base_address) {
        if(bus_controller->data_cmd_collection.read_write == BUS_RW_READ) 
            read_reg(pdevice,address,bus_controller->data_cmd_collection.be);
        else if(bus_controller->data_cmd_collection.read_write == BUS_RW_WRITE) 
            write_reg(pdevice,address,bus_controller->data_cmd_collection);
        return; //下一次循环，开始处理命令
    }
    
    switch (dev->regs[STDO_DEVICE_FUNC_REG]) {
        case 1:
            dev->bus_device->device_busy = 1;
            dev->regs[STDO_DEVICE_STAT_REG] = STDO_STAT_BUSY;
            device_output(dev,dev->regs[STDO_DEVICE_WRITE_BUF]);
            dev->regs[STDO_DEVICE_FUNC_REG] = 0;
            dev->bus_device->device_busy = 0;
            break;
        default:
            dev->regs[STDO_DEVICE_STAT_REG] = STDO_STAT_ERROR_UNKNOWN_CMD;
            break;
    }
    pthread_rwlock_unlock(&(dev->device_rwlock));
}
//只能调用一次
WOLF_CPU_BUS_DEVICE* init_stdo_device(WOLF_CPU_BUS_CONTROLLER* controller) {
    WOLF_CPU_BUS_DEVICE* bus_device = (WOLF_CPU_BUS_DEVICE*)calloc(1,sizeof(WOLF_CPU_BUS_DEVICE));
    WOLF_MMIO_STDO_DEVICE* stdo_device = (WOLF_MMIO_STDO_DEVICE*)calloc(1,sizeof(WOLF_MMIO_STDO_DEVICE));

    if(stdo_device == NULL || bus_device == NULL)
        return NULL; 
    memset(bus_device, 0 ,sizeof(WOLF_CPU_BUS_DEVICE));
    memset(stdo_device, 0, sizeof(WOLF_MMIO_STDO_DEVICE));

    strncpy(bus_device->name, device_name, DEVICE_NAME_STR_MAX);
    strncpy(bus_device->vendor, vendor_name,DEVICE_VENDOR_STR_MAX);
    bus_device->bus_controller = controller;
    bus_device->vendor_id = device_id;
    stdo_device->bus_device = bus_device;
    bus_device->need_space = need_space;
    bus_device->start_func = device_start;
    bus_device->vendor_id = device_id;
    bus_device->rd_reg_func = read_reg;
    bus_device->wr_reg_func = write_reg;
    pthread_rwlock_init(&(stdo_device->device_rwlock),NULL);
    return bus_device;
}
//只能调用一次
void destroy_stdo_device(WOLF_MMIO_STDO_DEVICE* stdo_device) { //设备结束运行时候调用的
    pthread_rwlock_destroy(&(stdo_device->device_rwlock));
    if(stdo_device != NULL) {
        if (stdo_device->bus_device != NULL) {
            free(stdo_device->bus_device);
        }
        free(stdo_device);
        stdo_device = NULL;
    }
}
