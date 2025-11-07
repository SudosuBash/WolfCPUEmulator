#include <controllers/mem_controller.h>
#include <stddef.h>
#include <ram/ram_eff.h>
#include <memory.h>
#define RAM_BLOCK_RECT_LOW_MASK (1<<(RAM_BLOCK_MAX_POSITION + RAM_BLOCK_MAX_POSITION))-1
#define RAM_BLOCK_RECT_HIGH_MASK (RAM_BLOCK_RECT_LOW_MASK ^ 0xffffffff)

typedef struct {
    uint32_t x;
    uint32_t y;
} RAM_BLOCK_POS;


RAM_RD_STATUS rd_memory(WOLF_MEM_CONTROLLER* controller,uint32_t paddr) {
    RAM_RD_STATUS stat = {0};

    RAM_IN_ARGS input = {0};
    input.opcode = RAM_IN_OPCODE_RD;
    input.opbytes = RAM_IN_OPBYTES_128BYTE;
    input.paddr = paddr;

    RAM_OPERATOR_RESULT result = controller->ram_unit->operatorFunc(controller->ram_unit,input);


    stat.dmem_error = result.status_flag & 1;
    if(!stat.dmem_error) {
        memcpy(stat.data.offset,result.data,32);
    }

    return stat;
}

RAM_RD_STATUS rd_memory_4b(WOLF_MEM_CONTROLLER* controller,uint32_t paddr) {
    RAM_RD_STATUS stat = {0};

    RAM_IN_ARGS input = {0};
    input.opcode = RAM_IN_OPCODE_RD;
    input.opbytes = RAM_IN_OPBYTES_4BYTE;
    input.paddr = paddr;

    RAM_OPERATOR_RESULT result = controller->ram_unit->operatorFunc(controller->ram_unit,input);

    stat.dmem_error = result.status_flag & 1;
    if(!stat.dmem_error) {
        memcpy(stat.data.offset,result.data,4);
    }

    return stat;
}

RAM_WR_STATUS wr_mem(WOLF_MEM_CONTROLLER* controller,uint32_t paddr,uint32_t value) {
    RAM_WR_STATUS stat = {0};

    RAM_IN_ARGS input = {0};
    input.opcode = RAM_IN_OPCODE_WR;
    input.opbytes = RAM_IN_OPBYTES_4BYTE;
    input.paddr = paddr;

    RAM_OPERATOR_RESULT result = controller->ram_unit->operatorFunc(controller->ram_unit,input);

    stat.dmem_error = result.status_flag & 1;
    if(!stat.dmem_error) {
        memcpy(&(stat.offset4),result.data,4);
    }
    return stat; 
}

WOLF_MEM_CONTROLLER* init_mem_controller() {
    WOLF_MEM_CONTROLLER* controller = (WOLF_MEM_CONTROLLER*)malloc(sizeof(WOLF_MEM_CONTROLLER));
    if(controller == NULL) return NULL;
    controller->rd_ram = rd_memory;
    controller->rd_ram_4b = rd_memory_4b;
    return controller;
}

void free_mem(WOLF_MEM_CONTROLLER** controller) {
    if(*controller != NULL) {
        free(*controller);
        *controller = NULL;
    }
}