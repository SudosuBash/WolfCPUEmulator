#ifndef __WOLF_STDO_DEVICE
#define __WOLF_STDO_DEVICE
#include "cpu/global.h"
#include <pthread.h>

#define STDO_DEVICE_FUNC_REG 0x0
#define STDO_DEVICE_STAT_REG 0x1
#define STDO_DEVICE_WRITE_BUF 0x2
#define STDO_DEVICE_REGS 5

#define STDO_STAT_OK 0
#define STDO_STAT_BUSY 0xff
#define STDO_STAT_ERROR_UNKNOWN 0xfe
#define STDO_STAT_ERROR_UNKNOWN_CMD 0x01
#define BUS_STDO_DEVICE_BASE_ADDR 0xF0000000

static const char device_name[] = "Wolf Basic Stdio Output";
static const char vendor_name[] = "Wolf Emulator";
static const uint8_t need_space = STDO_DEVICE_REGS;
static const uint16_t device_id = 0x1001;

typedef struct {
    WOLF_CPU_BUS_DEVICE* bus_device;
    uint8_t regs[STDO_DEVICE_REGS];
    pthread_rwlock_t device_rwlock;

    
} WOLF_MMIO_STDO_DEVICE;

uint8_t init_stdo_device();
void destroy_stdo_device();
#endif