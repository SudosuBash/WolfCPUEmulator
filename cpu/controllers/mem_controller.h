#ifndef __WOLF_MEM_CONTROLLER
#define __WOLF_MEM_CONTROLLER
#include "cpu/ram/ram.h"

typedef struct {
    uint8_t dmem_error:1;
    union {
        uint8_t val8;
        uint16_t val16;
        uint32_t val32;
    };
} RAM_RD_STATUS;


typedef RAM_RD_STATUS (*RD_MEMORY_C_1B)(RAM_INTERFACE_UNIT* unit,uint32_t paddr);
typedef RAM_RD_STATUS (*RD_MEMORY_C_2B)(RAM_INTERFACE_UNIT* unit,uint32_t paddr);
typedef RAM_RD_STATUS (*RD_MEMORY_C_4B)(RAM_INTERFACE_UNIT* unit,uint32_t paddr);
typedef uint8_t (*WR_MEMORY_C_1B)(RAM_INTERFACE_UNIT* unit,uint32_t paddr,uint8_t value);
typedef uint8_t (*WR_MEMORY_C_2B)(RAM_INTERFACE_UNIT* unit,uint32_t paddr,uint16_t value);
typedef uint8_t (*WR_MEMORY_C_4B)(RAM_INTERFACE_UNIT* unit,uint32_t paddr,uint32_t value);

typedef struct {
    RD_MEMORY_C_1B rd_ram_1b;
    RD_MEMORY_C_2B rd_ram_2b;
    RD_MEMORY_C_4B rd_ram_4b;
    
    WR_MEMORY_C_1B wr_ram_1b;
    WR_MEMORY_C_2B wr_ram_2b;
    WR_MEMORY_C_4B wr_ram_4b;
} WOLF_MEM_CONTROLLER;

#endif