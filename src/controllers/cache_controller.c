#include <controllers/cache_controller.h>
#include <logics/logic_alg.h>
#include <stdint.h>
#include <stddef.h>

#define L1_CACHE_LINE_WRITE(cnd,line1,data) \
    do { \
        (line1)->offset[0] = Mux32((cnd), (line1)->offset[0], (data)[0]);    \
        (line1)->offset[1] = Mux32((cnd), (line1)->offset[1], (data)[1]);    \
        (line1)->offset[2] = Mux32((cnd), (line1)->offset[2], (data)[2]);    \
        (line1)->offset[3] = Mux32((cnd), (line1)->offset[3], (data)[3]);    \
        (line1)->offset[4] = Mux32((cnd), (line1)->offset[4], (data)[4]);    \
        (line1)->offset[5] = Mux32((cnd), (line1)->offset[5], (data)[5]);    \
        (line1)->offset[6] = Mux32((cnd), (line1)->offset[6], (data)[6]);    \
        (line1)->offset[7] = Mux32((cnd), (line1)->offset[7], (data)[7]);    \
    } while(0)
#define L2_CACHE_MEM_LINE_WRITE(cnd,line1,data) \
    do { \
        (line1)->offset[0] = Mux32((cnd), (line1)->offset[0], (data)[0]);    \
        (line1)->offset[1] = Mux32((cnd), (line1)->offset[1], (data)[1]);    \
        (line1)->offset[2] = Mux32((cnd), (line1)->offset[2], (data)[2]);    \
        (line1)->offset[3] = Mux32((cnd), (line1)->offset[3], (data)[3]);    \
        (line1)->offset[4] = Mux32((cnd), (line1)->offset[4], (data)[4]);    \
        (line1)->offset[5] = Mux32((cnd), (line1)->offset[5], (data)[5]);    \
        (line1)->offset[6] = Mux32((cnd), (line1)->offset[6], (data)[6]);    \
        (line1)->offset[7] = Mux32((cnd), (line1)->offset[7], (data)[7]);    \
        (line1)->offset[8] = Mux32((cnd), (line1)->offset[8], (data)[8]);    \
        (line1)->offset[9] = Mux32((cnd), (line1)->offset[9], (data)[9]);    \
        (line1)->offset[10] = Mux32((cnd), (line1)->offset[10], (data)[10]);    \
        (line1)->offset[11] = Mux32((cnd), (line1)->offset[11], (data)[11]);    \
        (line1)->offset[12] = Mux32((cnd), (line1)->offset[12], (data)[12]);    \
        (line1)->offset[13] = Mux32((cnd), (line1)->offset[13], (data)[13]);    \
        (line1)->offset[14] = Mux32((cnd), (line1)->offset[14], (data)[14]);    \
        (line1)->offset[15] = Mux32((cnd), (line1)->offset[15], (data)[15]);    \
        (line1)->offset[16] = Mux32((cnd), (line1)->offset[16], (data)[16]);    \
        (line1)->offset[17] = Mux32((cnd), (line1)->offset[17], (data)[17]);    \
        (line1)->offset[18] = Mux32((cnd), (line1)->offset[18], (data)[18]);    \
        (line1)->offset[19] = Mux32((cnd), (line1)->offset[19], (data)[19]);    \
        (line1)->offset[20] = Mux32((cnd), (line1)->offset[20], (data)[20]);    \
        (line1)->offset[21] = Mux32((cnd), (line1)->offset[21], (data)[21]);    \
        (line1)->offset[22] = Mux32((cnd), (line1)->offset[22], (data)[22]);    \
        (line1)->offset[23] = Mux32((cnd), (line1)->offset[23], (data)[23]);    \
        (line1)->offset[24] = Mux32((cnd), (line1)->offset[24], (data)[24]);    \
        (line1)->offset[25] = Mux32((cnd), (line1)->offset[25], (data)[25]);    \
        (line1)->offset[26] = Mux32((cnd), (line1)->offset[26], (data)[26]);    \
        (line1)->offset[27] = Mux32((cnd), (line1)->offset[27], (data)[27]);    \
        (line1)->offset[28] = Mux32((cnd), (line1)->offset[28], (data)[28]);    \
        (line1)->offset[29] = Mux32((cnd), (line1)->offset[29], (data)[29]);    \
        (line1)->offset[30] = Mux32((cnd), (line1)->offset[30], (data)[30]);    \
        (line1)->offset[31] = Mux32((cnd), (line1)->offset[31], (data)[31]);    \
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
    stat.stat.addr_not_align = align != 0;
    if (stat.stat.addr_not_align) {
        return stat;
    }//不对齐的可以返回了，直接触发异常
    uint8_t offset = addr & (L2_OFFSET_MASK);
    uint16_t cgroup = (addr >> L2_OFFSET) & L2_GROUP_MASK;
    uint16_t tag = (addr >> L2_GROUP);

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
    stat.stat.addr_not_align = align != 0;
    if (stat.stat.addr_not_align) {
        return stat;
    }//不对齐的可以返回了，直接触发异常
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
uint8_t ld_cache_l1(MACHINE_L1_CACHE_GROUP* group[L1_GROUP_SIZE], uint32_t addr, uint32_t data[CACHE_ARR_SIZE(L1_SIZE, uint32_t)]) {
    uint8_t offset = addr & (L1_OFFSET_MASK);
    uint16_t cgroup = (addr >> L1_OFFSET) & L1_GROUP_MASK;
    uint16_t tag = (addr >> L1_GROUP);

    MACHINE_L1_CACHE_GROUP* targetGroup = group[cgroup];
    MACHINE_L1_CACHE_LINE* cacheLn1 = targetGroup->cache_lines_1;
    MACHINE_L1_CACHE_LINE* cacheLn2 = targetGroup->cache_lines_2;

    uint8_t L1OffsetWritten = (!CACHE_VALID(cacheLn1->valid))
        || (CACHE_VALID(cacheLn1->valid) && CACHE_LATEST_USED(cacheLn1->valid));
    L1_CACHE_LINE_WRITE(L1OffsetWritten, cacheLn1, data); //写入缓存1
    CHANGE_USED_STATUS(L1OffsetWritten, cacheLn1, cacheLn2);

    uint8_t L2OffsetWritten = (!CACHE_VALID(cacheLn2->valid) && CACHE_VALID(cacheLn1->valid)) || (
        CACHE_VALID(cacheLn2->valid) && CACHE_VALID(cacheLn1->valid) && CACHE_LATEST_USED(cacheLn2->valid)
    );
    L1_CACHE_LINE_WRITE(L2OffsetWritten, cacheLn2, data);
    CHANGE_USED_STATUS(L2OffsetWritten, cacheLn2, cacheLn1);
    return 0;
}



uint8_t ld_cache_l2(MACHINE_L2_CACHE_GROUP* group[L2_GROUP_SIZE], uint32_t addr, uint32_t data[CACHE_ARR_SIZE(L2_SIZE, uint32_t)]) {
    uint8_t offset = addr & (L2_OFFSET_MASK);
    uint16_t cgroup = (addr >> L2_OFFSET) & L1_GROUP_MASK;
    uint16_t tag = (addr >> L1_GROUP);

    MACHINE_L2_CACHE_GROUP* targetGroup = group[cgroup];
    MACHINE_L2_CACHE_LINE* cacheLn1 = targetGroup->cache_lines_1;
    MACHINE_L2_CACHE_LINE* cacheLn2 = targetGroup->cache_lines_2;

    uint8_t L1OffsetWritten = (!CACHE_VALID(cacheLn1->valid))
        || (CACHE_VALID(cacheLn1->valid) && CACHE_LATEST_USED(cacheLn1->valid));
    L2_CACHE_MEM_LINE_WRITE(L1OffsetWritten, cacheLn1, data); //写入缓存1
    CHANGE_USED_STATUS(L1OffsetWritten, cacheLn1, cacheLn2);

    uint8_t L2OffsetWritten = (!CACHE_VALID(cacheLn2->valid) && CACHE_VALID(cacheLn1->valid)) || (
        CACHE_VALID(cacheLn2->valid) && CACHE_VALID(cacheLn1->valid) && CACHE_LATEST_USED(cacheLn2->valid)
    );
    L2_CACHE_MEM_LINE_WRITE(L2OffsetWritten, cacheLn2, data);
    CHANGE_USED_STATUS(L2OffsetWritten, cacheLn2, cacheLn1);
    return 0;
}

WOLF_CACHE_CONTROLLER* init_cache_controller() {
    WOLF_CACHE_CONTROLLER* cache_controller = (WOLF_CACHE_CONTROLLER*) malloc(sizeof(WOLF_CACHE_CONTROLLER));
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