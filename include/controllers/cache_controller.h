#ifndef __WOLF_CACHE_CONTROLLER
#define __WOLF_CACHE_CONTROLLER

#include <stdint.h>
#include <controllers/cache.h>

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
    uint8_t reserved:2;
} CACHE_RD_STAT;

typedef struct {
    uint8_t offset[L1_SIZE];
    CACHE_RD_STAT stat;
    uint8_t relaAddr;
}L1_CACHE_RD_GROUP_RES;

typedef struct {
    uint8_t offset[L2_SIZE];
    CACHE_RD_STAT stat;
    uint8_t relaAddr;
}L2_CACHE_RD_GROUP_RES;

typedef uint8_t (*LD_CACHE_FN_L1)(MACHINE_L1_CACHE_GROUP* group[L1_GROUP_SIZE], uint32_t addr, uint8_t data[L1_SIZE]);
typedef uint8_t (*LD_CACHE_FN_L2)(MACHINE_L2_CACHE_GROUP* group[L2_GROUP_SIZE], uint32_t addr, uint8_t data[L2_SIZE]);

typedef L1_CACHE_RD_GROUP_RES (*RD_CACHE_GROUPS_L1)(MACHINE_L1_CACHE_GROUP* group[L1_GROUP_SIZE],uint32_t addr);
typedef L2_CACHE_RD_GROUP_RES (*RD_CACHE_GROUPS_L2)(MACHINE_L2_CACHE_GROUP* group[L2_GROUP_SIZE],uint32_t addr);

typedef struct {
    RD_CACHE_GROUPS_L1 rd_l1_groups;
    RD_CACHE_GROUPS_L2 rd_l2_groups;

    LD_CACHE_FN_L1 ld_l1_cache;
    LD_CACHE_FN_L2 ld_l2_cache;
} WOLF_CACHE_CONTROLLER;

WOLF_CACHE_CONTROLLER* init_cache_controller();
void free_cache(WOLF_CACHE_CONTROLLER** cache);
#endif