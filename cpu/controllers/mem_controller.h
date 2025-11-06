#ifndef __WOLF_MEM_CONTROLLER
#define __WOLF_MEM_CONTROLLER
#include "cpu/ram/ram.h"

typedef struct {
    uint8_t dmem_error:1;
    union {
        uint32_t offset[L2_SIZE / sizeof(uint32_t)];
        uint32_t offset4;
    } data;
} RAM_RD_STATUS;

typedef RAM_RD_STATUS (*RD_MEMORY_C)(WOLF_MEM_CONTROLLER* controller,uint32_t paddr);
typedef uint8_t (*WR_MEMORY_C)(WOLF_MEM_CONTROLLER* controller,uint32_t paddr,uint32_t value);

typedef struct {
    RAM_INTERFACE_UNIT* ram_unit;
    RD_MEMORY_C rd_ram;
    RD_MEMORY_C rd_ram_4b;
    WR_MEMORY_C wr_ram;
} WOLF_MEM_CONTROLLER;

WOLF_MEM_CONTROLLER* init_mem_controller();
void free_mem(WOLF_MEM_CONTROLLER** controller);
#endif