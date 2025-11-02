#include "cache_controller.h"
#include "cpu/logics/logic_alg.h"
#include <stdint.h>

#define L1_CACHE_LINE_WRITE(cnd,line1,data) \
    do { \
        (line1)->offset[0] = Mux64((cnd), (line1)->offset[0], (data)[0]);    \
        (line1)->offset[1] = Mux64((cnd), (line1)->offset[1], (data)[1]);    \
        (line1)->offset[2] = Mux64((cnd), (line1)->offset[2], (data)[2]);    \
        (line1)->offset[3] = Mux64((cnd), (line1)->offset[3], (data)[3]);    \
    } while(0)
#define L2_CACHE_LINE_WRITE(cnd,line1,data) \
    do { \
        (line1)->offset[0] = Mux64((cnd), (line1)->offset[0], (data)[0]);    \
        (line1)->offset[1] = Mux64((cnd), (line1)->offset[1], (data)[1]);    \
        (line1)->offset[2] = Mux64((cnd), (line1)->offset[2], (data)[2]);    \
        (line1)->offset[3] = Mux64((cnd), (line1)->offset[3], (data)[3]);    \
        (line1)->offset[4] = Mux64((cnd), (line1)->offset[4], (data)[4]);    \
        (line1)->offset[5] = Mux64((cnd), (line1)->offset[5], (data)[5]);    \
        (line1)->offset[6] = Mux64((cnd), (line1)->offset[6], (data)[6]);    \
        (line1)->offset[7] = Mux64((cnd), (line1)->offset[7], (data)[7]);    \
        (line1)->offset[8] = Mux64((cnd), (line1)->offset[8], (data)[8]);    \
        (line1)->offset[9] = Mux64((cnd), (line1)->offset[9], (data)[9]);    \
        (line1)->offset[10] = Mux64((cnd), (line1)->offset[10], (data)[10]);    \
        (line1)->offset[11] = Mux64((cnd), (line1)->offset[11], (data)[11]);    \
        (line1)->offset[12] = Mux64((cnd), (line1)->offset[12], (data)[12]);    \
        (line1)->offset[13] = Mux64((cnd), (line1)->offset[13], (data)[13]);    \
        (line1)->offset[14] = Mux64((cnd), (line1)->offset[14], (data)[14]);    \
        (line1)->offset[15] = Mux64((cnd), (line1)->offset[15], (data)[15]);    \
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



CACHE_RD_STATUS rd_1b_l1(MACHINE_L1_CACHE_GROUP* group[L1_GROUP_SIZE],uint32_t addr) {
    CACHE_RD_STATUS stat;
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
    stat.val8 = Through8(cacheLine2Hit, cacheLn2->offset[offset]) | Through8(cacheLine1Hit, cacheLn2->offset[offset]);
    return stat;
}

CACHE_RD_STATUS rd_4b_l1(MACHINE_L1_CACHE_GROUP* group[L1_GROUP_SIZE],uint32_t addr) {
    CACHE_RD_STATUS stat;
    uint8_t align = addr & 3;
    stat.stat.addr_not_align = align != 0;
    if (stat.stat.addr_not_align) {
        return stat;
    }//不对齐的可以返回了，直接触发异常
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
    stat.val32 = Through32(cacheLine2Hit, GET_DATA_32(cacheLn2->offset, offset)) | Through32(cacheLine1Hit,GET_DATA_32(cacheLn1->offset, offset));
    return stat;
}

CACHE_RD_STATUS rd_2b_l1(MACHINE_L1_CACHE_GROUP* group[L1_GROUP_SIZE],uint32_t addr) {
    CACHE_RD_STATUS stat;
    uint8_t align = addr & 1;
    stat.stat.addr_not_align = align != 0;
    if (stat.stat.addr_not_align) {
        return stat;
    }//不对齐的可以返回了，直接触发异常
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
    stat.val16 = Through16(cacheLine2Hit, GET_DATA_16(cacheLn2->offset, offset)) | 
                Through16(cacheLine1Hit,GET_DATA_16(cacheLn1->offset, offset));
    return stat;
}

CACHE_RD_STATUS rd_1b_l2(MACHINE_L2_CACHE_GROUP* group[L2_GROUP_SIZE],uint32_t addr) {
    CACHE_RD_STATUS stat;
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
    stat.stat.hit = cacheLine1Hit || cacheLine2Hit;
    stat.val8 = Through8(cacheLine2Hit, cacheLn2->offset[offset]) | Through8(cacheLine1Hit, cacheLn2->offset[offset]);
    return stat;
}

CACHE_RD_STATUS rd_4b_l2(MACHINE_L2_CACHE_GROUP* group[L2_GROUP_SIZE],uint32_t addr) {
    CACHE_RD_STATUS stat;
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
    stat.stat.hit = cacheLine1Hit || cacheLine2Hit;
    stat.val32 = Through32(cacheLine2Hit, GET_DATA_32(cacheLn2->offset, offset)) | Through32(cacheLine1Hit,GET_DATA_32(cacheLn1->offset, offset));
    return stat;
}

CACHE_RD_STATUS rd_2b_l2(MACHINE_L2_CACHE_GROUP* group[L2_GROUP_SIZE],uint32_t addr) {
    CACHE_RD_STATUS stat;
    uint8_t align = addr & 1;
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

    stat.stat.hit = cacheLine1Hit || cacheLine2Hit;
    stat.val16 = Through16(cacheLine2Hit, GET_DATA_16(cacheLn2->offset, offset)) | 
                Through16(cacheLine1Hit,GET_DATA_16(cacheLn1->offset, offset));
    return stat;
}

uint8_t ld_cache_l1(MACHINE_L1_CACHE_GROUP* group[L1_GROUP_SIZE],uint32_t addr, uint64_t data[CACHE_ARR_SIZE(L1_SIZE, uint64_t)]) {
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


uint8_t ld_cache_l2(MACHINE_L2_CACHE_GROUP* group[L1_GROUP_SIZE],uint32_t addr, uint64_t data[CACHE_ARR_SIZE(L1_SIZE, uint64_t)]) {
    uint8_t offset = addr & (L2_OFFSET_MASK);
    uint16_t cgroup = (addr >> L2_OFFSET) & L1_GROUP_MASK;
    uint16_t tag = (addr >> L1_GROUP);

    MACHINE_L2_CACHE_GROUP* targetGroup = group[cgroup];
    MACHINE_L2_CACHE_LINE* cacheLn1 = targetGroup->cache_lines_1;
    MACHINE_L2_CACHE_LINE* cacheLn2 = targetGroup->cache_lines_2;

    uint8_t L1OffsetWritten = (!CACHE_VALID(cacheLn1->valid))
        || (CACHE_VALID(cacheLn1->valid) && CACHE_LATEST_USED(cacheLn1->valid));
    L2_CACHE_LINE_WRITE(L1OffsetWritten, cacheLn1, data); //写入缓存1
    CHANGE_USED_STATUS(L1OffsetWritten, cacheLn1, cacheLn2);

    uint8_t L2OffsetWritten = (!CACHE_VALID(cacheLn2->valid) && CACHE_VALID(cacheLn1->valid)) || (
        CACHE_VALID(cacheLn2->valid) && CACHE_VALID(cacheLn1->valid) && CACHE_LATEST_USED(cacheLn2->valid)
    );
    L2_CACHE_LINE_WRITE(L2OffsetWritten, cacheLn2, data);
    CHANGE_USED_STATUS(L2OffsetWritten, cacheLn2, cacheLn1);
    return 0;
}

void init_cache_controller(WOLF_CACHE_CONTROLLER* cache_controller) {
    cache_controller->rd_1b_l1 = rd_1b_l1;
    cache_controller->rd_4b_l1 = rd_4b_l1;
    cache_controller->rd_2b_l1 = rd_2b_l1;
    cache_controller->rd_1b_l2 = rd_1b_l2;
    cache_controller->rd_2b_l2 = rd_2b_l2;
    cache_controller->rd_4b_l2 = rd_4b_l2;
    cache_controller->ld_l1_cache = ld_cache_l1;
    cache_controller->ld_l2_cache = ld_cache_l2;
}