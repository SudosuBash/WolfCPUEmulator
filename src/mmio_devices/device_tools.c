#include <mmio_devices/device_tools.h>
#include <logics/logic_alg.h>
#include <stdio.h>
uint8_t read_reg_general(WOLF_CPU_BUS_CONTROLLER* controller,uint32_t addr,uint32_t base_address, uint8_t max_reg_size, uint8_t* regs) {
    uint8_t be = controller->data_cmd_collection.be;
    uint8_t rd_0 = (be & 1) == 1;
    uint8_t rd_1 = (be & 2) == 1;
    uint8_t rd_2 = (be & 4) == 1;
    uint8_t rd_3 = (be & 8) == 1;
    uint8_t rel_addr = addr - base_address;
    uint8_t rel_addr1 = rel_addr + Mux8(rd_1, 0, 1);
    uint8_t rel_addr2 = (rel_addr) + Mux8(rd_2,0,2);
    uint8_t rel_addr3 = (rel_addr) + Mux8(rd_3,0,3);

    if(rel_addr3 >= max_reg_size ||
        rel_addr2 >= max_reg_size ||
        rel_addr1 >=max_reg_size ||
        rel_addr >= max_reg_size) {

        pthread_mutex_lock(&controller->device_request_mutex);

        controller->data_cmd_collection.status = BUS_STATUS_ERROR;
        pthread_cond_signal(&controller->device_request_mutex_cond);
        pthread_mutex_unlock(&controller->device_request_mutex);
        return STAT_UNKNOWN_ERROR;
    }
    uint8_t dat1 = Mux8(rd_0,0,regs[rel_addr]);
    uint8_t dat2 = Mux8(rd_1,0,regs[rel_addr1]);
    uint8_t dat3 = Mux8(rd_2,0,regs[rel_addr2]);
    uint8_t dat4 = Mux8(rd_3,0,regs[rel_addr3]);
    controller->data_cmd_collection.data = (dat4 << 24) | (dat3 << 16) | (dat2 << 8) | (dat1);
    pthread_mutex_lock(&controller->device_request_mutex);

    controller->data_cmd_collection.status = BUS_STATUS_SUCCESS;
    pthread_cond_signal(&controller->device_request_mutex_cond);
    pthread_mutex_unlock(&controller->device_request_mutex);
    return STAT_SUCCESS;
}

uint8_t write_reg_general(WOLF_CPU_BUS_CONTROLLER* controller,uint32_t addr,uint8_t base_address,uint8_t max_reg_size, uint8_t* regs) {
    uint8_t be = controller->data_cmd_collection.be;
    uint32_t data = controller->data_cmd_collection.data;

    uint8_t wri_0 = (be & 1) == 1;
    uint8_t wri_1 = (be >> 1) & 1;
    uint8_t wri_2 = (be >> 2) & 1;
    uint8_t wri_3 = (be >> 3) & 1;
    uint8_t rel_addr = addr - base_address;
    uint8_t rel_addr1 = rel_addr + Mux8(wri_1, 0, 1);
    uint8_t rel_addr2 = (rel_addr) + Mux8(wri_2,0,2);
    uint8_t rel_addr3 = (rel_addr) + Mux8(wri_3,0,3);

    if(rel_addr3 >= max_reg_size ||
        rel_addr2 >= max_reg_size ||
        rel_addr1 >= max_reg_size ||
        rel_addr >= max_reg_size) {

        pthread_mutex_lock(&controller->device_request_mutex);
        controller->data_cmd_collection.status = BUS_STATUS_ERROR;
        pthread_cond_signal(&controller->device_request_mutex_cond);
        pthread_mutex_unlock(&controller->device_request_mutex);
        return STAT_UNKNOWN_ERROR;
    }

    regs[rel_addr] = Mux8(wri_0,regs[rel_addr],GET_DATA_0(data));
    regs[rel_addr1] = Mux8(wri_1,regs[rel_addr1], GET_DATA_1(data));
    regs[rel_addr2] = Mux8(wri_2,regs[rel_addr2], GET_DATA_2(data));
    regs[rel_addr3] = Mux8(wri_3,regs[rel_addr3], GET_DATA_3(data));
    pthread_mutex_lock(&controller->device_request_mutex);

    controller->data_cmd_collection.status = BUS_STATUS_SUCCESS;
    pthread_cond_signal(&controller->device_request_mutex_cond);
    pthread_mutex_unlock(&controller->device_request_mutex);
    return STAT_SUCCESS;
}

void reset_bus(WOLF_CPU_BUS_CONTROLLER* bus_controller) {
    bus_controller->data_cmd_collection.be = 0;
    bus_controller->data_cmd_collection.data = 0;//重置
    bus_controller->addr = 0;
}