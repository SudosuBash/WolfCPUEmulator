#include <controllers/cache.h>
#include <stddef.h>
#include <global.h>
MACHINE_L1_CACHE_GROUP** init_l1_group(uint32_t groups) {
    MACHINE_L1_CACHE_GROUP** l1cache = (MACHINE_L1_CACHE_GROUP**) calloc(groups,sizeof(MACHINE_L1_CACHE_GROUP*));
    memset(l1cache,0,sizeof(l1cache));
    for(int i=0;i<groups;i++) {
        MACHINE_L1_CACHE_GROUP* c1 = (MACHINE_L1_CACHE_GROUP*)calloc(1,sizeof(MACHINE_L1_CACHE_GROUP));
        l1cache[i] = c1;
        MACHINE_L1_CACHE_LINE* line1 = (MACHINE_L1_CACHE_LINE*) calloc(1,sizeof(MACHINE_L1_CACHE_LINE));
        MACHINE_L1_CACHE_LINE* line2 = (MACHINE_L1_CACHE_LINE*) calloc(1,sizeof(MACHINE_L1_CACHE_LINE));

        memset(line1,0,sizeof(MACHINE_L1_CACHE_LINE));
        memset(line2,0,sizeof(MACHINE_L1_CACHE_LINE));

        c1->cache_lines_1 = line1;
        c1->cache_lines_2 = line2;
    }
    return l1cache;
}

MACHINE_L2_CACHE_GROUP** init_l2_group(uint32_t groups) {
    MACHINE_L2_CACHE_GROUP** l1cache = (MACHINE_L2_CACHE_GROUP**) calloc(groups,sizeof(MACHINE_L2_CACHE_GROUP*));
    memset(l1cache,0,sizeof(l1cache));
    for(int i=0;i<groups;i++) {
        MACHINE_L2_CACHE_GROUP* c1 = (MACHINE_L2_CACHE_GROUP*)calloc(1,sizeof(MACHINE_L2_CACHE_GROUP));
        l1cache[i] = c1;
        MACHINE_L2_CACHE_LINE* line1 = (MACHINE_L2_CACHE_LINE*) calloc(1,sizeof(MACHINE_L2_CACHE_LINE));
        MACHINE_L2_CACHE_LINE* line2 = (MACHINE_L2_CACHE_LINE*) calloc(1,sizeof(MACHINE_L2_CACHE_LINE));

        memset(line1,0,sizeof(MACHINE_L2_CACHE_LINE));
        memset(line2,0,sizeof(MACHINE_L2_CACHE_LINE));

        c1->cache_lines_1 = line1;
        c1->cache_lines_2 = line2;
    }
    return l1cache;
}

void free_l1_group(MACHINE_L1_CACHE_GROUP*** lcache,uint32_t groups) {
    MACHINE_L1_CACHE_GROUP** lcache_arrs = *lcache;
    if(lcache == NULL) {
        for(int i=0;i<groups;i++) {
            free((*lcache_arrs)->cache_lines_1);
            free((*lcache_arrs)->cache_lines_2);
            free(*lcache_arrs);
        }
        free(*lcache);
        *lcache = NULL;
    }

}

void free_l2_group(MACHINE_L2_CACHE_GROUP*** lcache,uint32_t groups) {
    MACHINE_L2_CACHE_GROUP** lcache_arrs = *lcache;
    if(lcache == NULL) {
        for(int i=0;i<groups;i++) {
            free((*lcache_arrs)->cache_lines_1);
            free((*lcache_arrs)->cache_lines_2);
            free(*lcache_arrs);
        }
        free(*lcache);
        *lcache = NULL;
    }
}