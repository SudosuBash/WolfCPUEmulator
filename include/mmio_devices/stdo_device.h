#ifndef __WOLF_STDO_DEVICE
#define __WOLF_STDO_DEVICE
#include <global.h>
#include <pthread.h>

#define STDO_DEVICE_FUNC_REG 0x0
#define STDO_DEVICE_STAT_REG 0x1
#define STDO_DEVICE_WRITE_BUF 0x2
#define STDO_DEVICE_REGS 8

#define STDO_STAT_OK 0
#define STDO_STAT_BUSY 0xff
#define STDO_STAT_ERROR_UNKNOWN 0xfe
#define STDO_STAT_ERROR_UNKNOWN_CMD 0x01

#include <stdint.h>
#include <controllers/bus.h>

typedef struct {
    uint8_t regs[STDO_DEVICE_REGS];
    WOLF_CPU_BUS_DEVICE* bus_device;

    pthread_mutex_t stdio_lock;
} WOLF_MMIO_STDO_DEVICE,*PWOLF_MMIO_STDO_DEVICE;

PWOLF_CPU_BUS_DEVICE* init_stdo_device(WOLF_CPU_BUS_CONTROLLER* controller);
void destroy_stdo_device(PWOLF_MMIO_STDO_DEVICE* pstdo_device);
#endif