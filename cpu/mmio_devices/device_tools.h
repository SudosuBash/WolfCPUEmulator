#ifndef __WOLF_DEVICE_TOOLS
#define __WOLF_DEVICE_TOOLS

#include <stdint.h>
#include <cpu/global.h>

uint8_t read_reg_general(uint32_t addr,uint8_t be, uint32_t base_address, uint8_t max_reg_size, uint8_t* regs, uint32_t* value);
uint8_t write_reg_general(uint32_t addr,uint8_t be,uint32_t data,uint8_t base_address,uint8_t max_reg_size, uint8_t* regs);
#endif