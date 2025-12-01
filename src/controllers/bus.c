#include <controllers/bus.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <global.h>
#include <tools/endian.h>
#include <unistd.h>
#include <errno.h>
uint8_t bus_send_data(PWOLF_CPU_BUS_CONTROLLER* pbus_ctrl, uint32_t addr,BUS_SEND_DATA data) {
    WOLF_CPU_BUS_CONTROLLER* bus_ctrl = *pbus_ctrl;

    pthread_mutex_lock(&bus_ctrl->busy_mutex);
    data.status = BUS_STATUS_PENDING;
    data.read_write = BUS_RW_WRITE;
    bus_ctrl->data_cmd_collection = data;
    bus_ctrl->addr = addr;
    //for循环可比锁竞争快多了
    for(int i=0;i<MAX_BUS_DEVICE;i++) {
        if(bus_ctrl->devices[i] == NULL) continue;
        if(bus_ctrl->devices[i]->base_address <= addr && addr < bus_ctrl->devices[i]->base_address+bus_ctrl->devices[i]->need_space) {
            pthread_cond_signal(&bus_ctrl->devices[i]->device_op_signal);
            break;
        }
    }
    pthread_mutex_unlock(&bus_ctrl->busy_mutex);

    pthread_mutex_lock(&bus_ctrl->device_request_mutex);
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_nsec += BUS_WAIT_DELTA * 1000 * 1000;//外设等待0.5s
    while(bus_ctrl->data_cmd_collection.status==BUS_STATUS_PENDING) {
        int res = pthread_cond_timedwait(&bus_ctrl->device_request_mutex_cond,&bus_ctrl->device_request_mutex,&timeout);
        if(res==ETIMEDOUT) {
            bus_ctrl->data_cmd_collection.status = BUS_STATUS_TIMEOUT;
            break;
        }
    }
   pthread_mutex_unlock(&bus_ctrl->device_request_mutex);

#ifdef _EMU_DEBUG
    fflush(stdout);
    printf("Status = %d\n",bus_ctrl->data_cmd_collection.status);
#endif
    uint8_t status = bus_ctrl->data_cmd_collection.status;
    
    if(bus_ctrl->data_cmd_collection.status == BUS_STATUS_TIMEOUT) {
        status = BUS_STATUS_TIMEOUT;
        return status;
    }
    return status;
}

BUS_SEND_DATA bus_recv_data(PWOLF_CPU_BUS_CONTROLLER* pbus_ctrl, uint32_t addr,BUS_SEND_DATA data) {
    WOLF_CPU_BUS_CONTROLLER* bus_ctrl = *pbus_ctrl;

    pthread_mutex_lock(&bus_ctrl->busy_mutex);
    data.status = BUS_STATUS_PENDING;
    data.read_write = BUS_RW_READ;
    bus_ctrl->data_cmd_collection = data;
    bus_ctrl->addr = addr;
    //for循环可比锁竞争快多了
    for(int i=0;i<MAX_BUS_DEVICE;i++) {
        if(bus_ctrl->devices[i] == NULL) continue;
        if(bus_ctrl->devices[i]->base_address <= addr && addr < bus_ctrl->devices[i]->base_address+bus_ctrl->devices[i]->need_space) {
            pthread_cond_signal(&bus_ctrl->devices[i]->device_op_signal);
            break;
        }
    }
    pthread_mutex_unlock(&bus_ctrl->busy_mutex); 
    //广播设备

    pthread_mutex_lock(&bus_ctrl->device_request_mutex);
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec +=BUS_WAIT_DELTA / 1000;//外设等待0.5s
    int res = pthread_cond_timedwait(&bus_ctrl->device_request_mutex_cond,&bus_ctrl->device_request_mutex,&timeout);
    pthread_mutex_unlock(&bus_ctrl->device_request_mutex);

    COPY_BYTE_4_ARRAY(data.data,bus_ctrl->data_cmd_collection.data);
    data.status = bus_ctrl->data_cmd_collection.status;

    if(bus_ctrl->data_cmd_collection.status == BUS_STATUS_PENDING) {
        bus_ctrl->data_cmd_collection.status = BUS_STATUS_TIMEOUT; //得不到响应，设定超时
        data.status = BUS_STATUS_TIMEOUT;
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
    pthread_mutex_init(&(controller->device_request_mutex),NULL);
    pthread_cond_init(&(controller->device_request_mutex_cond),NULL);
    
    pthread_mutex_init(&(controller->busy_mutex),NULL);
    //后续实现register_devices 
    return controller;
}

void free_bus(WOLF_CPU_BUS_CONTROLLER** bus) {
    if(*bus != NULL) {
        pthread_mutex_destroy(&(*bus)->device_request_mutex);
        pthread_cond_destroy(&(*bus)->device_request_mutex_cond);
        pthread_mutex_destroy(&(*bus)->busy_mutex);
        free(*bus);
        *bus = NULL;
    }
}

void register_device(PWOLF_CPU_BUS_CONTROLLER* bus_ctrl,WOLF_CPU_BUS_DEVICE* device,uint8_t device_position) {
    PWOLF_CPU_BUS_CONTROLLER controller = *bus_ctrl;
    controller->devices[device_position] = device;
}