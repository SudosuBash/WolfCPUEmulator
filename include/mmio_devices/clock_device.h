#ifndef __WOLF_CLOCK_DEVICE
#define __WOLF_CLOCK_DEVICE
#include <global.h>
#include <pthread.h>

#define CLOCK_DEVICE_REGS 4
#define CLOCK_DEVICE_OPEN_ADDR 0
#define CLOCK_DEVICE_WAIT_DELTA 1
#include <stdint.h>
#include <controllers/bus.h>
#include <mmio_devices/irq_controller.h>
//模拟简易的时钟中断
typedef struct {
    uint8_t regs[CLOCK_DEVICE_REGS];
    WOLF_CPU_BUS_DEVICE* bus_device;

    pthread_rwlock_t device_rwlock;
} WOLF_MMIO_CLOCK_DEVICE,*PWOLF_MMIO_CLOCK_DEVICE;

PWOLF_CPU_BUS_DEVICE* init_clock_device(WOLF_CPU_BUS_CONTROLLER* controller,WOLF_IRQ_CONTROLLER* irq);
void destroy_clock_device(PWOLF_MMIO_CLOCK_DEVICE* pstdo_device);
#endif