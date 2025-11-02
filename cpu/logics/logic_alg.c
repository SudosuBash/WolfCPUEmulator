#include "logic_alg.h"
#include <stdint.h>

uint32_t Through32(uint8_t cnd, uint32_t value) {
    uint32_t cnd32 = (uint32_t)(cnd);
    cnd32 = -cnd32;
    return cnd32 & value;
}

uint16_t Through16(uint8_t cnd, uint16_t value) {
    uint16_t cnd16 = (uint16_t)(cnd);
    cnd16 = -cnd16;
    return cnd16 & value;
}

uint64_t Through64(uint8_t cnd, uint64_t value) {
    uint64_t cnd64 = (uint64_t)(cnd);
    cnd64 = -cnd64;
    return cnd64 & value;
}
uint8_t Through8(uint8_t cnd,uint8_t value) {
    uint8_t cnd8=-cnd;
    return cnd8 & value;
}

uint64_t Mux64(uint8_t cnd, uint64_t origin,uint64_t newValue) {
    uint64_t cnd64 = (uint64_t)(cnd); // 0
    uint64_t cnd64mask = -cnd64;
    uint64_t fcnd64 = cnd64 ^ 1;
    uint64_t fcnd64mask = -fcnd64;
    return (cnd64mask & newValue) | (fcnd64mask & origin);
}

uint8_t Mux8(uint8_t cnd, uint8_t origin,uint8_t newValue) {
    uint8_t cnd8 = (uint64_t)(cnd); // 0
    uint8_t cnd8mask = -cnd8;
    uint8_t fcnd8 = cnd8 ^ 1;
    uint8_t fcnd8mask = -fcnd8;
    return (cnd8mask & newValue) | (fcnd8mask & origin);
}