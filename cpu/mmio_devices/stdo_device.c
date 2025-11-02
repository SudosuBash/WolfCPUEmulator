#include "stdo_device.h"
#include "device_tools.h"

#include <cpu/logics/logic_alg.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <cpu/controllers/bus.h>

#define GET_DATA_0(data) ((data) & 0xff)
#define GET_DATA_1(data) ((data) >> 8) & 0xff
#define GET_DATA_2(data) ((data) >> 16) & 0xff
#define GET_DATA_3(data) ((data) >> 24)

static WOLF_MMIO_STDO_DEVICE* stdo_device;
static void write_reg(WOLF_CPU_BUS_DEVICE* device,uint8_t addr,BUS_SEND_DATA data) {
    WOLF_MMIO_STDO_DEVICE* dev = get_parent_struct(device,WOLF_MMIO_STDO_DEVICE, bus_device);
    pthread_rwlock_wrlock(&(dev->device_rwlock));
    uint8_t stat = write_reg_general(addr,data.be,data.data,device->base_address,STDO_DEVICE_REGS,dev->regs);
    if(stat == 0) {
        dev->bus_device->device_base_status = 1;
        pthread_rwlock_unlock(&(dev->device_rwlock));
        return;
    }
    dev->bus_device->device_base_status = 0;
    pthread_rwlock_unlock(&(dev->device_rwlock));
}

static uint32_t read_reg(WOLF_CPU_BUS_DEVICE* device,uint8_t addr,uint8_t be) {

    WOLF_MMIO_STDO_DEVICE* dev = get_parent_struct(device,WOLF_MMIO_STDO_DEVICE,bus_device);
    pthread_rwlock_rdlock(&(dev->device_rwlock));

    uint32_t value;
    uint8_t stat = read_reg_general(addr,be,device->base_address,STDO_DEVICE_REGS,dev->regs,&value);

    if(stat == 0) {
        dev->bus_device->device_base_status = 1;
        pthread_rwlock_unlock(&(dev->device_rwlock));
        return 0;
    }
    pthread_rwlock_unlock(&(dev->device_rwlock));
    return value;
}

static void device_output(WOLF_MMIO_STDO_DEVICE* device,uint8_t write_buf) {
    printf("%c",(char)(write_buf));
    device->regs[STDO_DEVICE_STAT_REG] = STDO_STAT_OK;
}

static void device_start(WOLF_CPU_BUS_DEVICE* stdo_device_desc) {
    WOLF_MMIO_STDO_DEVICE* dev = get_parent_struct(stdo_device_desc,WOLF_MMIO_STDO_DEVICE,bus_device);
    pthread_rwlock_wrlock(&(dev->device_rwlock));
    if(dev->bus_device->device_busy) { //这用if纯粹是为了效率，否则后面的也会执行
        pthread_rwlock_unlock(&(dev->device_rwlock));
        return;
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
uint8_t init_stdo_device() {
    WOLF_CPU_BUS_DEVICE* bus_device = (WOLF_CPU_BUS_DEVICE*)malloc(sizeof(WOLF_CPU_BUS_DEVICE));
    stdo_device = (WOLF_MMIO_STDO_DEVICE*)malloc(sizeof(WOLF_MMIO_STDO_DEVICE));

    if(stdo_device == NULL || bus_device == NULL)
        return -1; 
    memset(bus_device, 0 ,sizeof(WOLF_CPU_BUS_DEVICE));
    memset(stdo_device, 0, sizeof(WOLF_MMIO_STDO_DEVICE));

    strncpy(bus_device->name, device_name, DEVICE_NAME_STR_MAX);
    strncpy(bus_device->vendor, vendor_name,DEVICE_VENDOR_STR_MAX);
    bus_device->vendor_id = device_id;
    bus_device->base_address = BUS_STDO_DEVICE_BASE_ADDR;
    stdo_device->bus_device = bus_device;
    bus_device->need_space = need_space;
    bus_device->start_func = device_start;
    bus_device->vendor_id = device_id;
    bus_device->rd_reg_func = read_reg;
    bus_device->wr_reg_func = write_reg;
    pthread_rwlock_init(&(stdo_device->device_rwlock),NULL);
    return 0;
}
//只能调用一次
void destroy_stdo_device() { //设备结束运行时候调用的
    pthread_rwlock_destroy(&(stdo_device->device_rwlock));
    if(stdo_device != NULL) {
        if (stdo_device->bus_device != NULL) {
            free(stdo_device->bus_device);
        }
        free(stdo_device);
        stdo_device = NULL;
    }
}
