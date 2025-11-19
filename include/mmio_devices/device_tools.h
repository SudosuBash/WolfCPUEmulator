#ifndef __WOLF_DEVICE_TOOLS
#define __WOLF_DEVICE_TOOLS

#include <stdint.h>
#include <global.h>
#include <controllers/bus.h>

uint8_t read_reg_general(WOLF_CPU_BUS_CONTROLLER* controller,uint32_t addr,uint32_t base_address, uint8_t max_reg_size, uint8_t* regs);
uint8_t write_reg_general(WOLF_CPU_BUS_CONTROLLER* controller,uint32_t addr,uint8_t base_address,uint8_t max_reg_size, uint8_t* regs);
void reset_bus(WOLF_CPU_BUS_CONTROLLER* bus_controller);

#define PROCESS_DEVICE_REGISTER_WRITING(device) \
    do { \
        if((device)->bus_controller->data_cmd_collection.read_write == BUS_RW_READ) \
            read_reg(pdevice,(device)->bus_controller->addr,(device)->bus_controller->data_cmd_collection.be); \
        else if((device)->bus_controller->data_cmd_collection.read_write == BUS_RW_WRITE) \
            write_reg(pdevice,(device)->bus_controller->addr,(device)->bus_controller->data_cmd_collection); \
        reset_bus((device)->bus_controller); \
    }while(0) \

#define WAIT_FOR_BUS_WAKE_UP(device) \
    do { \
        pthread_mutex_lock(&(device)->bus_controller->busy_mutex); \
        while (!((device)->bus_controller->addr < (device)->base_address + device->need_space && (device)->bus_controller->addr >= (device)->base_address))  { \
            pthread_cond_wait(&(device)->bus_controller->busy_cond,&(device)->bus_controller->busy_mutex); \
        } \
        pthread_mutex_unlock(&(device)->bus_controller->busy_mutex);   \
    } while(0);

    
#define LOOP_CMP_IF_WRITE_TO_REG(device) \
    do {\
        if((device)->bus_controller->addr < (device)->base_address + device->need_space && (device)->bus_controller->addr >= (device)->base_address)  { \
            PROCESS_DEVICE_REGISTER_WRITING(device); \
        } \
    } while(0)

#define INIT_BUS_DEVICE(bus_device,pname,pvendor,controller,pvendor_id,pneed_space,pstart_func,prd_reg_func,pwr_reg_func) \
    do { \
        strncpy((bus_device)->name, (pname), DEVICE_NAME_STR_MAX); \
        strncpy((bus_device)->vendor, (pvendor),DEVICE_VENDOR_STR_MAX); \
            (bus_device)->bus_controller = (controller); \
            (bus_device)->vendor_id = (pvendor_id); \
        (bus_device)->need_space = (pneed_space); \
        (bus_device)->start_func = (pstart_func); \
        (bus_device)->rd_reg_func = (prd_reg_func); \
        (bus_device)->wr_reg_func = (pwr_reg_func); \
    } while(0);


#endif
