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

#define STAT_SUCCESS 0
#define STAT_UNKNOWN_ERROR 0xff

#define GET_DATA_0(data) ((data) & 0xff)
#define GET_DATA_1(data) ((data) >> 8) & 0xff
#define GET_DATA_2(data) ((data) >> 16) & 0xff
#define GET_DATA_3(data) ((data) >> 24)

#define GET_DATA_BIT0(data) ((data) & 0x1)
#define GET_DATA_BIT1(data) (((data) >> 1) & 0x1)
#define GET_DATA_BIT2(data) (((data) >> 2) & 0x1)
#define GET_DATA_BIT3(data) (((data) >> 3) & 0x1)

#define BE_ALIGN(data) \
   BE_DATA(data) - 1

#define EXFLAG_OPERATE_4_BITS 0b00
#define EXFLAG_OPERATE_2_BITS_LOW 0b10
#define EXFLAG_OPERATE_1_BITS 0b01
#define EXFLAG_OPERATE_2_BITS_HIGH 0b11

#define BE_DATA(data) \
   (Through8((data)==0b1,1) | \
   Through8((data)==0b11,2) | \
   Through8((data)==0b1111 || (data) == 0b0011,4))

#define ICODE_EXFLAG_MOV_BE(data) \
   (Through32((data)==EXFLAG_OPERATE_1_BITS,0b1) | \
   Through32((data)==EXFLAG_OPERATE_2_BITS_LOW,0b11) | \
   Through32((data)==EXFLAG_OPERATE_4_BITS,0b1111) | \
   Through32((data)==EXFLAG_OPERATE_2_BITS_HIGH,0b0011))

//通过EXFLAG获取对应的掩码
#define DATA32_MASK_BE(origin,dest,be) ( \
   Mux8((be) & 0x1,GET_DATA_0(origin),GET_DATA_0(dest)) | \
   (Mux8(((be) >> 1) & 0x1,GET_DATA_1(origin), GET_DATA_1(dest)) << 8) | \
   (Mux8(((be) >> 2) & 0x1,GET_DATA_2(origin), GET_DATA_2(dest)) << 16) | \
   (Mux8(((be) >> 3) & 0x1,GET_DATA_3(origin), GET_DATA_3(dest)) << 24) \
)
//通过be获取对应的值

//这种直接打表了,作用: 获取非4字节对齐对应最接近的4字节对齐的地址的Be
//例如: 地址0x2,访问2字节返回结果: 0b0011
//地址0x2,访问1字节返回结果: 0b0010
// #define _EMU_DEBUG 1
// #define _EMU_IRQ_TEST_DEBUG 1
#define _EMU_MMIO_DEBUG 1
#endif