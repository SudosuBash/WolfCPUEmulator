#ifndef __CPU_GLOBAL
#define __CPU_GLOBAL
#include <stdlib.h>
#include <string.h>
#include <logics/logic_alg.h>
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

#define CACHE_L1_GROUPS 512
#define CACHE_L2_GROUPS 1024

#define get_parent_struct(object,type,member) \
   (type*)((char*)(object) - offsetof(type, member))
#define malloc(size) calloc(1,size)
#define KERN_MODE_MASK 0x01
#define CACHE_OPEN_MASK 0x02
#define BCR_PGO_MASK 0x04

#define STAT_SUCCESS 0
#define STAT_UNKNOWN_ERROR 0xff

#define GET_DATA_0(data) ((data) & 0xff)
#define GET_DATA_1(data) ((data) >> 8) & 0xff
#define GET_DATA_2(data) ((data) >> 16) & 0xff
#define GET_DATA_3(data) ((data) >> 24)


#define BE_DATA(data) \
   (Through8((data)==0b1,1) | \
   Through8((data)==0b11,2) | \
   Through8((data)==0b1111,4))

#define BE_ALIGN(data) \
   BE_DATA(data) - 1
#define BE_MASK_GEN(data) \
   (Through32((data)==0b1,0xff) | \
   Through32((data)==0b10,0xffff) | \
   Through32((data)==0b00,0xffffffff))

#define DEBUG_ON 1

#endif