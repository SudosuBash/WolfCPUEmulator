#ifndef _WOLF_RAM_EFF
#define _WOLF_RAM_EFF
#include <stdint.h>
#include <global.h>

#define RAM_SIZE 4294967296

#define RAM_OFFSET_BITS 14
#define RAM_PTE_BITS 8
#define RAM_PDE_BITS 10

#define RAM_PTE_POS RAM_OFFSET_BITS
#define RAM_PDE_POS (RAM_OFFSET_BITS + RAM_PTE_POS)

#define RAM_PDE_ITEM (1 << RAM_PDE_BITS)
#define RAM_PTE_ITEM (1 << RAM_PTE_BITS)
#define RAM_SINGLE_BLK_SIZE (1 << RAM_OFFSET_BITS)

#define RAM_PTE_MASK (RAM_PTE_ITEM - 1)
#define RAM_OFFSET_MASK (RAM_SINGLE_BLK_SIZE - 1)

#define RAM_IN_OPCODE_RD 0
#define RAM_IN_OPCODE_WR 1

#define RAM_IN_OPBYTES_128BYTE 0
#define RAM_IN_OPBYTES_4BYTE 1
//512 MB
typedef struct {
    uint8_t status_flag:1; //就一位，1代表内存超限
    uint32_t data[L2_GROUP_SIZE / sizeof(uint32_t)];
} RAM_OPERATOR_RESULT;

typedef struct {
    uint8_t opcode:2;
    uint8_t opbytes:1; 
    //若是WRITE操作,此位忽略
    uint32_t paddr;
    uint32_t val4bIn;
} RAM_IN_ARGS;

typedef struct RAM_INTERFACE_UNIT RAM_INTERFACE_UNIT;
typedef RAM_OPERATOR_RESULT (*ram_operator_func)(RAM_INTERFACE_UNIT* unit,RAM_IN_ARGS input);

struct RAM_INTERFACE_UNIT {
    uint32_t* ram_page_table[RAM_PDE_ITEM][RAM_PTE_ITEM];
    //就占2MB,挺小的
    //需要的时候再分配
    ram_operator_func operatorFunc;
};

RAM_INTERFACE_UNIT* init_ram_unit();
void free_ram_unit(RAM_INTERFACE_UNIT* unit);
#endif