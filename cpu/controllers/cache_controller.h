#ifndef __WOLF_CACHE_CONTROLLER
#define __WOLF_CACHE_CONTROLLER

#include <stdint.h>
#include "cpu/controllers/cache.h"

#define CACHE_ARR_SIZE(csize, type) \
    (csize / (sizeof(type)))

#define GET_DATA_32(arr,offset) \
    (( (arr)[offset+3] << 24) | ( (arr)[offset+2] << 16) | ( (arr)[offset+1] << 8) | ((arr)[offset]))
#define GET_DATA_16(arr,offset) \
    (( ((arr)[offset+1]) << 8) | ((arr)[offset]))
#define CACHE_VALID_MASK 1
#define CACHE_LATEST_USED_MASK 2

typedef struct {
    uint8_t hit:1;
    uint8_t addr_not_align:1;
    uint8_t reserved:2;
} CACHE_RD_STAT;
typedef struct {
    CACHE_RD_STAT stat;
    union {
        uint8_t val8;
        uint16_t val16;
        uint32_t val32;
    };
} CACHE_RD_STATUS;

typedef CACHE_RD_STATUS (*RD_CACHE_FN_L1)(MACHINE_L1_CACHE_GROUP* group[L1_GROUP_SIZE],uint32_t addr);
typedef CACHE_RD_STATUS (*RD_CACHE_FN_L2)(MACHINE_L2_CACHE_GROUP* group[L2_GROUP_SIZE],uint32_t addr);
typedef uint8_t (*LD_CACHE_FN_L1)(MACHINE_L1_CACHE_GROUP* group[L1_GROUP_SIZE], uint32_t addr, uint64_t data[CACHE_ARR_SIZE(L1_SIZE, uint64_t)]);
typedef uint8_t (*LD_CACHE_FN_L2)(MACHINE_L2_CACHE_GROUP* group[L2_GROUP_SIZE], uint32_t addr, uint64_t data[CACHE_ARR_SIZE(L2_SIZE, uint64_t)]);


typedef struct {
    RD_CACHE_FN_L1 rd_1b_l1;
    RD_CACHE_FN_L1 rd_2b_l1;
    RD_CACHE_FN_L1 rd_4b_l1;
    RD_CACHE_FN_L2 rd_1b_l2;
    RD_CACHE_FN_L2 rd_2b_l2;
    RD_CACHE_FN_L2 rd_4b_l2;

    LD_CACHE_FN_L1 ld_l1_cache;
    LD_CACHE_FN_L2 ld_l2_cache;
} WOLF_CACHE_CONTROLLER;
#endif