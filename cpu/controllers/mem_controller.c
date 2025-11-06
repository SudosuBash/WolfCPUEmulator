#include "mem_controller.h"
#include <cpu/ram/ram.h>
#include <stddef.h>

#define RAM_BLOCK_RECT_LOW_MASK (1<<(RAM_BLOCK_MAX_POSITION + RAM_BLOCK_MAX_POSITION))-1
#define RAM_BLOCK_RECT_HIGH_MASK (RAM_BLOCK_RECT_LOW_MASK ^ 0xffffffff)

typedef struct {
    uint32_t x;
    uint32_t y;
} RAM_BLOCK_POS;


RAM_RD_STATUS rd_memory(WOLF_MEM_CONTROLLER* controller,uint32_t paddr) {
    RAM_RD_STATUS stat = {0};
    uint32_t blockx = paddr & (RAM_BLOCK_RECT_WIDTH - 1);
    uint32_t blocky = (paddr & RAM_BLOCK_RECT_LOW_MASK) >> RAM_BLOCK_MAX_POSITION;
    
    RAM_IN_ARGS input = {0};
    input.opcode |= 1;
    input.op_x = blockx;
    input.op_y = blocky;
    input.opbytes = 4;

    RAM_OPERATOR_RESULT result = controller->ram_unit->operatorFunc(controller->ram_unit,input);
    stat.dmem_error = result.status_flag & 1;
    
    return stat;
}

WOLF_MEM_CONTROLLER* init_mem_controller() {
    WOLF_MEM_CONTROLLER* controller = (WOLF_MEM_CONTROLLER*)malloc(sizeof(WOLF_MEM_CONTROLLER*));
    if(controller == NULL) return NULL;
    controller->rd_ram = rd_memory;
    return controller;
}

void free_mem(WOLF_MEM_CONTROLLER** controller) {
    if(*controller != NULL) {
        free(*controller);
        *controller = NULL;
    }
}