#ifndef __WOLF_MEM_CONTROLLER
#define __WOLF_MEM_CONTROLLER
#include <global.h>
#include <ram/ram_eff.h>
typedef struct WOLF_MEM_CONTROLLER WOLF_MEM_CONTROLLER,*PWOLF_MEM_CONTROLLER;
typedef struct {
    uint8_t dmem_error:1;
    union {
        uint32_t offset[L2_SIZE / sizeof(uint32_t)];
        uint32_t offset4;
    } data;
} RAM_RD_STATUS;

typedef struct {
    uint8_t dmem_error:1;
    uint32_t offset4;
} RAM_WR_STATUS;
typedef RAM_RD_STATUS (*RD_MEMORY_C)(PWOLF_MEM_CONTROLLER* controller,uint32_t paddr);
typedef RAM_WR_STATUS (*WR_MEMORY_C)(PWOLF_MEM_CONTROLLER* controller,uint32_t paddr,uint32_t value);

struct WOLF_MEM_CONTROLLER {
    RAM_INTERFACE_UNIT* ram_unit;
    RD_MEMORY_C rd_ram;
    RD_MEMORY_C rd_ram_4b;
    WR_MEMORY_C wr_ram;
};

WOLF_MEM_CONTROLLER* init_mem_controller();
void free_mem(WOLF_MEM_CONTROLLER** controller);
#endif