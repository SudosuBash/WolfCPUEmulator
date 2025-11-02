#ifndef __LOGIC_ALG
#define __LOGIC_ALG

#include <stdint.h>

/**
 * Cnd = 0/1.
 * 
 * 30
 */
uint8_t Through8(uint8_t cnd,uint8_t value);
uint16_t Through16(uint8_t cnd, uint16_t value);
uint64_t Through64(uint8_t cnd,uint64_t value);
uint32_t Through32(uint8_t cnd,uint32_t value);

uint64_t Mux64(uint8_t cnd,uint64_t origin,uint64_t current);
uint8_t Mux8(uint8_t cnd,uint8_t origin,uint8_t current);
#endif