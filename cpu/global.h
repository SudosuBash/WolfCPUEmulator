#ifndef __CPU_GLOBAL
#define __CPU_GLOBAL

#define CPU_OPLENG 32

#define L1_TAG 18
#define L1_OFFSET 5
#define L1_GROUP (CPU_OPLENG - L1_TAG - L1_OFFSET)
#define L1_GROUP_SIZE (1 << L1_GROUP)
#define L1_GROUP_MASK (L1_GROUP_SIZE - 1)
#define L1_SIZE (1 << L1_OFFSET)
#define L1_OFFSET_MASK L1_SIZE-1

#define L2_TAG 15
#define L2_OFFSET 7
#define L2_GROUP (CPU_OPLENG - L2_TAG - L2_OFFSET)
#define L2_GROUP_SIZE (1 << L2_GROUP)
#define L2_GROUP_MASK (L2_GROUP_SIZE - 1)
#define L2_SIZE (1 << L2_OFFSET)
#define L2_OFFSET_MASK L2_SIZE-1

#define RAM_BLOCK_MAX_POSITION 14
#define RAM_BLOCK_RECT_WIDTH (1 << RAM_BLOCK_MAX_POSITION)
#define RAM_BLOCK_RECT_SIZE RAM_BLOCK_RECT_WIDTH*RAM_BLOCK_RECT_WIDTH
#define RAM_BLOCK_COUNT 8
#define CACHE_L1_GROUPS 512
#define CACHE_L2_GROUPS 1024

#define get_parent_struct(object,type,member) \
   (type*)((char*)(object) - (size_t)(&(((type*)0)->member)))
#define KERN_MODE_MASK 0x01
#define CACHE_OPEN_MASK 0x02
#define BCR_PGO_MASK 0x04

#define STAT_SUCCESS 0
#define STAT_UNKNOWN_ERROR 0xff
#endif