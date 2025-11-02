#ifndef __WOLF_CACHE_L1
#define __WOLF_CACHE_L1

#include <stdint.h>
#include "cpu/global.h"
typedef struct {
    uint8_t valid:4;
    uint32_t tag:L1_TAG;
    uint64_t offset[L1_SIZE / sizeof(uint64_t)];
} MACHINE_L1_CACHE_LINE;

typedef struct {
    MACHINE_L1_CACHE_LINE* cache_lines_1;
    MACHINE_L1_CACHE_LINE* cache_lines_2;
} MACHINE_L1_CACHE_GROUP;
//L1 Cache:
// 32 KB
// 32 bytes per line;
// 2 lines per group

typedef struct {
    uint8_t valid:4;
    uint32_t tag:L2_TAG;
    uint64_t offset[L2_SIZE / sizeof(uint64_t)];
} MACHINE_L2_CACHE_LINE;

typedef struct {
    MACHINE_L2_CACHE_LINE* cache_lines_1;
    MACHINE_L2_CACHE_LINE* cache_lines_2;
} MACHINE_L2_CACHE_GROUP;
//L2 Cache:
// 256 KB;
// 128 bytes per line;
// 2 lines per group

#endif