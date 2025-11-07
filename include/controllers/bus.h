#ifndef _WOLF_CPU_BUS
#define _WOLF_CPU_BUS
#include <stdint.h>
#include <pthread.h>


#define DEVICE_VENDOR_STR_MAX 32
#define DEVICE_NAME_STR_MAX 32
#define MAX_BUS_DEVICE 256

#define BUS_STATUS_ERROR 0x2
#define BUS_STATUS_SUCCESS 0x1
#define BUS_STATUS_PENDING 0x0
#define BUS_STATUS_TIMEOUT 0x3

#define BUS_RW_READ 0x0
#define BUS_RW_WRITE 0x1

#define BUS_WAIT_DELTA 3000
typedef struct {
    uint32_t data;
    uint8_t be:4;
    uint8_t status:2;
    uint8_t read_write:1
}BUS_SEND_DATA;

typedef struct WOLF_CPU_BUS_DEVICE WOLF_CPU_BUS_DEVICE;
typedef struct WOLF_CPU_BUS_CONTROLLER WOLF_CPU_BUS_CONTROLLER;

typedef void (*bus_reg_device_fn)(WOLF_CPU_BUS_CONTROLLER* bus_ctrl);
typedef uint8_t (*bus_send_data_fn)(WOLF_CPU_BUS_CONTROLLER* bus_ctrl, uint32_t addr,BUS_SEND_DATA data);
typedef BUS_SEND_DATA (*bus_recv_data_fn)(WOLF_CPU_BUS_CONTROLLER* bus_ctrl, uint32_t addr,BUS_SEND_DATA bits);


typedef void (*device_start_fn)(WOLF_CPU_BUS_DEVICE* device);

typedef void (*device_read_reg_fn)(WOLF_CPU_BUS_DEVICE* device,uint8_t addr,uint8_t be);
typedef void (*device_write_reg_fn)(WOLF_CPU_BUS_DEVICE* device,uint8_t addr,BUS_SEND_DATA data);

struct WOLF_CPU_BUS_CONTROLLER {
    uint32_t addr;
    BUS_SEND_DATA data_cmd_collection;
    WOLF_CPU_BUS_DEVICE* devices[MAX_BUS_DEVICE];
    bus_send_data_fn send_data;
    bus_recv_data_fn recv_data;

    //以下是模拟硬件连接到总线的功能，真实的硬件不存在这个函数，初始化的时候搞定它
    bus_reg_device_fn register_devices;
};

struct WOLF_CPU_BUS_DEVICE {
    WOLF_CPU_BUS_CONTROLLER* bus_controller;
    uint32_t base_address; //MMIO地址
    char vendor[32];
    char name[32];
    uint16_t vendor_id;
    uint8_t need_space;
    uint8_t device_base_status;
    uint32_t irq_number; //传递中断号
    uint8_t device_busy:2;

    device_start_fn start_func; 
    device_write_reg_fn wr_reg_func;
    device_read_reg_fn rd_reg_func;
    //机器启动的时候，会新开一个线程，while执行start_func函数，模拟启动这个设备
    //所有的设备处理都在这个函数里面
    //输入输出在自定义寄存器里面(bar+need_reg_space)，由设备自己定义
};

WOLF_CPU_BUS_CONTROLLER* init_bus();
void free_bus(WOLF_CPU_BUS_CONTROLLER** bus);
#endif