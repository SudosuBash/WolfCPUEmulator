#include <controllers/cache_controller.h>
#include <logics/logic_alg.h>
#include <stdint.h>
#include <stddef.h>

#define L1_CACHE_LINE_WRITE(cnd,line1,data) \
    do { \
        for(uint8_t i=0;i<16;i+=4) { \
            (line1)->offset[i] = Mux8((cnd), (line1)->offset[i], (data)[i]);    \
            (line1)->offset[i+1] = Mux8((cnd), (line1)->offset[i+1], (data)[i+1]);    \
            (line1)->offset[i+2] = Mux8((cnd), (line1)->offset[i+2], (data)[i+2]);    \
            (line1)->offset[i+3] = Mux8((cnd), (line1)->offset[i+3], (data)[i+3]);    \
        } \
    } while(0)
#define L2_CACHE_MEM_LINE_WRITE(cnd,line1,data) \
    do { \
        for(uint8_t i=0;i<128;i+=4) { \
            (line1)->offset[i] = Mux8((cnd), (line1)->offset[i], (data)[i]);    \
            (line1)->offset[i+1] = Mux8((cnd), (line1)->offset[i+1], (data)[i+1]);    \
            (line1)->offset[i+2] = Mux8((cnd), (line1)->offset[i+2], (data)[i+2]);    \
            (line1)->offset[i+3] = Mux8((cnd), (line1)->offset[i+3], (data)[i+3]);    \
        } \
    } while(0)
#define CHANGE_USED_STATUS(replace,line1,line2) \
    do { \
        (line1)->valid = Mux8(replace,(line1)->valid,(line1)->valid | (CACHE_VALID_MASK | CACHE_LATEST_USED_MASK));               \
        (line2)->valid = Mux8(replace,(line2)->valid,(line2)->valid & (0b1111 ^ CACHE_LATEST_USED_MASK));                       \
    } while(0)

#define CACHE_VALID(value) \
    ((value) & CACHE_VALID_MASK)

#define CACHE_LATEST_USED(value) \
    ((value) & CACHE_LATEST_USED_MASK)

/**
 * offset: uint64_t*
 * data: uint64_t[32]
 */

L1_CACHE_RD_GROUP_RES rd_cache_l1_groups(MACHINE_L1_CACHE_GROUP* group[L1_GROUP_SIZE],uint32_t addr) {
    L1_CACHE_RD_GROUP_RES stat = {0};
    uint8_t align = addr & 3;
    uint8_t offset = addr & (L1_OFFSET_MASK);
    uint16_t cgroup = (addr >> L1_OFFSET) & L1_GROUP_MASK;
    uint16_t tag = (addr >> L1_GROUP);

    MACHINE_L1_CACHE_GROUP* targetGroup = group[cgroup];

    MACHINE_L1_CACHE_LINE* cacheLn1 = targetGroup->cache_lines_1;
    MACHINE_L1_CACHE_LINE* cacheLn2 = targetGroup->cache_lines_2;

    uint8_t cacheLine1Hit = (cacheLn1->tag == tag && cacheLn1->valid);
    uint8_t cacheLine2Hit = (cacheLn2->tag == tag && cacheLn2->valid);
    CHANGE_USED_STATUS(cacheLine1Hit,cacheLn1, cacheLn2);
    CHANGE_USED_STATUS(cacheLine2Hit, cacheLn2, cacheLn1);
    stat.stat.hit = cacheLine1Hit || cacheLine2Hit;
    L1_CACHE_LINE_WRITE(cacheLine1Hit,&stat,cacheLn1->offset);
    L1_CACHE_LINE_WRITE(cacheLine2Hit,&stat,cacheLn2->offset);
    stat.relaAddr = addr & (L1_OFFSET_MASK);
    return stat;
}

L2_CACHE_RD_GROUP_RES rd_cache_l2_groups(MACHINE_L2_CACHE_GROUP* group[L2_GROUP_SIZE],uint32_t addr) {
    L2_CACHE_RD_GROUP_RES stat = {0};
    uint8_t align = addr & 3;
    uint8_t offset = addr & (L2_OFFSET_MASK);
    uint16_t cgroup = (addr >> L2_OFFSET) & L2_GROUP_MASK;
    uint16_t tag = (addr >> L2_GROUP);

    MACHINE_L2_CACHE_GROUP* targetGroup = group[cgroup];

    MACHINE_L2_CACHE_LINE* cacheLn1 = targetGroup->cache_lines_1;
    MACHINE_L2_CACHE_LINE* cacheLn2 = targetGroup->cache_lines_2;

    uint8_t cacheLine1Hit = (cacheLn1->tag == tag && cacheLn1->valid);
    uint8_t cacheLine2Hit = (cacheLn2->tag == tag && cacheLn2->valid);
    CHANGE_USED_STATUS(cacheLine1Hit,cacheLn1, cacheLn2);
    CHANGE_USED_STATUS(cacheLine2Hit, cacheLn2, cacheLn1);
    L2_CACHE_MEM_LINE_WRITE(cacheLine1Hit,&stat,cacheLn1->offset);
    L2_CACHE_MEM_LINE_WRITE(cacheLine2Hit,&stat,cacheLn2->offset);
    stat.stat.hit = cacheLine1Hit || cacheLine2Hit;
    stat.relaAddr = addr & (L2_OFFSET_MASK);
    return stat;
}
uint8_t ld_cache_l1(MACHINE_L1_CACHE_GROUP* group[L1_GROUP_SIZE], uint32_t addr, uint8_t data[L1_SIZE]) {
    uint8_t offset = addr & (L1_OFFSET_MASK);
    uint16_t cgroup = (addr >> L1_OFFSET) & L1_GROUP_MASK;
    uint16_t tag = (addr >> L1_GROUP);

    MACHINE_L1_CACHE_GROUP* targetGroup = group[cgroup];
    MACHINE_L1_CACHE_LINE* cacheLn1 = targetGroup->cache_lines_1;
    MACHINE_L1_CACHE_LINE* cacheLn2 = targetGroup->cache_lines_2;

    uint8_t L1OffsetWritten = (!CACHE_VALID(cacheLn1->valid))
        || (CACHE_VALID(cacheLn1->valid) && CACHE_LATEST_USED(cacheLn1->valid));
    L1_CACHE_LINE_WRITE(L1OffsetWritten, cacheLn1, data); //写入缓存1
    cacheLn1->tag = Mux32(L1OffsetWritten,cacheLn1->tag,tag);
    CHANGE_USED_STATUS(L1OffsetWritten, cacheLn1, cacheLn2);

    uint8_t L2OffsetWritten = (!CACHE_VALID(cacheLn2->valid) && CACHE_VALID(cacheLn1->valid)) || (
        CACHE_VALID(cacheLn2->valid) && CACHE_VALID(cacheLn1->valid) && CACHE_LATEST_USED(cacheLn2->valid)
    );
    L1_CACHE_LINE_WRITE(L2OffsetWritten, cacheLn2, data);
    cacheLn1->tag = Mux32(L2OffsetWritten,cacheLn2->tag,tag);
    CHANGE_USED_STATUS(L2OffsetWritten, cacheLn2, cacheLn1);
    return 0;
}



uint8_t ld_cache_l2(MACHINE_L2_CACHE_GROUP* group[L2_GROUP_SIZE], uint32_t addr, uint8_t data[L2_SIZE]) {
    uint8_t offset = addr & (L2_OFFSET_MASK);
    uint16_t cgroup = (addr >> L2_OFFSET) & L2_GROUP_MASK;
    uint16_t tag = (addr >> L2_GROUP);

    MACHINE_L2_CACHE_GROUP* targetGroup = group[cgroup];
    MACHINE_L2_CACHE_LINE* cacheLn1 = targetGroup->cache_lines_1;
    MACHINE_L2_CACHE_LINE* cacheLn2 = targetGroup->cache_lines_2;

    uint8_t L1OffsetWritten = (!CACHE_VALID(cacheLn1->valid))
        || (CACHE_VALID(cacheLn1->valid) && CACHE_LATEST_USED(cacheLn1->valid));
    L2_CACHE_MEM_LINE_WRITE(L1OffsetWritten, cacheLn1, data); //写入缓存1
    cacheLn1->tag = Mux32(L1OffsetWritten,cacheLn2->tag,tag);
    CHANGE_USED_STATUS(L1OffsetWritten, cacheLn1, cacheLn2);

    uint8_t L2OffsetWritten = (!CACHE_VALID(cacheLn2->valid) && CACHE_VALID(cacheLn1->valid)) || (
        CACHE_VALID(cacheLn2->valid) && CACHE_VALID(cacheLn1->valid) && CACHE_LATEST_USED(cacheLn2->valid)
    );
    L2_CACHE_MEM_LINE_WRITE(L2OffsetWritten, cacheLn2, data);
    cacheLn1->tag = Mux32(L2OffsetWritten,cacheLn2->tag,tag);
    CHANGE_USED_STATUS(L2OffsetWritten, cacheLn2, cacheLn1);
    return 0;
}

WOLF_CACHE_CONTROLLER* init_cache_controller() {
    WOLF_CACHE_CONTROLLER* cache_controller = (WOLF_CACHE_CONTROLLER*) calloc(1,sizeof(WOLF_CACHE_CONTROLLER));
    cache_controller->ld_l1_cache = ld_cache_l1;
    cache_controller->ld_l2_cache = ld_cache_l2;
    cache_controller->rd_l1_groups = rd_cache_l1_groups;
    cache_controller->rd_l2_groups = rd_cache_l2_groups;
    return cache_controller;
}

void free_cache(WOLF_CACHE_CONTROLLER** cache) {
    if(*cache != NULL) {
        free(*cache);
        *cache = NULL;
    }
}