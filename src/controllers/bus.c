#include <controllers/bus.h>
#include <string.h>
#include <stdlib.h>
uint8_t bus_send_data(PWOLF_CPU_BUS_CONTROLLER* pbus_ctrl, uint32_t addr,BUS_SEND_DATA data) {
    WOLF_CPU_BUS_CONTROLLER* bus_ctrl = *pbus_ctrl;
    data.status = BUS_STATUS_PENDING;
    data.read_write = BUS_RW_WRITE;
    bus_ctrl->data_cmd_collection = data;
    bus_ctrl->addr = addr;
    
    uint16_t wait_delta = 0;
    while(bus_ctrl->data_cmd_collection.status != BUS_STATUS_PENDING && wait_delta < BUS_WAIT_DELTA) {
        wait_delta ++;
    }
    bus_ctrl->data_cmd_collection.be = 0;
    bus_ctrl->data_cmd_collection.data = 0;//重置

    uint8_t status = bus_ctrl->data_cmd_collection.status;
    
    if(bus_ctrl->data_cmd_collection.status == BUS_STATUS_PENDING) {
        bus_ctrl->data_cmd_collection.status = BUS_STATUS_TIMEOUT; //得不到响应，设定超时  
        return status;
    }
    bus_ctrl->data_cmd_collection.status = BUS_STATUS_SUCCESS;
    status = bus_ctrl->data_cmd_collection.status;
    return status;
}

BUS_SEND_DATA bus_recv_data(PWOLF_CPU_BUS_CONTROLLER* pbus_ctrl, uint32_t addr,BUS_SEND_DATA data) {
    WOLF_CPU_BUS_CONTROLLER* bus_ctrl = *pbus_ctrl;
    data.status = BUS_STATUS_PENDING;
    data.read_write = BUS_RW_READ;

    bus_ctrl->data_cmd_collection = data;
    bus_ctrl->addr = addr;

    uint16_t wait_delta = 0;
    while(bus_ctrl->data_cmd_collection.status != BUS_STATUS_PENDING && wait_delta < BUS_WAIT_DELTA) {
        wait_delta ++;
    }

    data.data = bus_ctrl->data_cmd_collection.data;
    data.status = bus_ctrl->data_cmd_collection.status;

    bus_ctrl->data_cmd_collection.be = 0;
    if(bus_ctrl->data_cmd_collection.status == BUS_STATUS_PENDING) {
        bus_ctrl->data_cmd_collection.status = BUS_STATUS_TIMEOUT; //得不到响应，设定超时  
        return data;
    }
    bus_ctrl->data_cmd_collection.status = BUS_STATUS_SUCCESS;
    return data;
}

WOLF_CPU_BUS_CONTROLLER* init_bus() {
    WOLF_CPU_BUS_CONTROLLER* controller = (WOLF_CPU_BUS_CONTROLLER*)calloc(1,sizeof(WOLF_CPU_BUS_CONTROLLER));
    if(controller == NULL) return NULL;

    controller->send_data = bus_send_data;
    controller->recv_data = bus_recv_data;
    //后续实现register_devices 
    return controller;
}

void free_bus(WOLF_CPU_BUS_CONTROLLER** bus) {
    if(*bus != NULL) {
        free(*bus);
        *bus = NULL;
    }
}

void register_device(PWOLF_CPU_BUS_CONTROLLER* bus_ctrl,WOLF_CPU_BUS_DEVICE* device,uint8_t device_position) {
    PWOLF_CPU_BUS_CONTROLLER controller = *bus_ctrl;
    controller->devices[device_position] = device;
}