#ifndef __WOLF_MEM_CONTROLLER
#define __WOLF_MEM_CONTROLLER
#include <global.h>
#include <ram/ram_eff.h>
typedef struct WOLF_MEM_CONTROLLER WOLF_MEM_CONTROLLER,*PWOLF_MEM_CONTROLLER;
typedef struct {
    uint8_t dmem_error:1;
    union {
        uint8_t offset[L2_SIZE];
        uint8_t offset4[4];
    } data;
} RAM_RD_STATUS;

typedef struct {
    uint8_t dmem_error:1;
    uint8_t offset4[4];
} RAM_WR_STATUS;
typedef RAM_RD_STATUS (*RD_MEMORY_C)(PWOLF_MEM_CONTROLLER* controller,uint32_t paddr);
typedef RAM_RD_STATUS (*RD_MEMORY_C_SINGLE)(PWOLF_MEM_CONTROLLER* controller,uint32_t paddr,uint8_t be);
typedef RAM_WR_STATUS (*WR_MEMORY_C)(PWOLF_MEM_CONTROLLER* controller,uint32_t paddr,uint8_t value[4],uint8_t be);

struct WOLF_MEM_CONTROLLER {
    RAM_INTERFACE_UNIT* ram_unit;
    RD_MEMORY_C rd_ram;
    RD_MEMORY_C_SINGLE rd_ram_4b;
    WR_MEMORY_C wr_ram;
};

WOLF_MEM_CONTROLLER* init_mem_controller();
void free_mem(WOLF_MEM_CONTROLLER** controller);
#endif