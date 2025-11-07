#ifndef __WOLF_DEVICE_TOOLS
#define __WOLF_DEVICE_TOOLS

#include <stdint.h>
#include <global.h>
#include <controllers/bus.h>

uint8_t read_reg_general(WOLF_CPU_BUS_CONTROLLER* controller,uint32_t addr,uint32_t base_address, uint8_t max_reg_size, uint8_t* regs);
uint8_t write_reg_general(WOLF_CPU_BUS_CONTROLLER* controller,uint32_t addr,uint8_t base_address,uint8_t max_reg_size, uint8_t* regs);
#endif
