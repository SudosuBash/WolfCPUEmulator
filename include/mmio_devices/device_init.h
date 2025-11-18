#ifndef _WOLF_DEVICE_INIT
#define _WOLF_DEVICE_INIT

#include <stdint.h>
#include <cpu.h>
#define DEVICE_INIT_STATUS_SUCCESS 0x0
#define DEVICE_INIT_STATUS_ERROR 0xff
typedef uint8_t INIT_STATUS;
INIT_STATUS init_devices(WOLF_CPU* cpu);
void reset_bus(WOLF_CPU_BUS_CONTROLLER* bus_controller);
#endif