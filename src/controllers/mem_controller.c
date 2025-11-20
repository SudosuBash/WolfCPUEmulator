#include <controllers/mem_controller.h>
#include <stddef.h>
#include <ram/ram_eff.h>
#include <memory.h>
#include <tools.h>
#define RAM_BLOCK_RECT_LOW_MASK (1<<(RAM_BLOCK_MAX_POSITION + RAM_BLOCK_MAX_POSITION))-1
#define RAM_BLOCK_RECT_HIGH_MASK (RAM_BLOCK_RECT_LOW_MASK ^ 0xffffffff)

typedef struct {
    uint32_t x;
    uint32_t y;
} RAM_BLOCK_POS;


RAM_RD_STATUS rd_memory(PWOLF_MEM_CONTROLLER* controller,uint32_t paddr) {
    RAM_RD_STATUS stat = {0};

    RAM_IN_ARGS input = {0};
    input.opcode = RAM_IN_OPCODE_RD;
    input.opbytes = RAM_IN_OPBYTES_128BYTE;
    input.paddr = paddr;
    RAM_OPERATOR_RESULT result = (*controller)->ram_unit->operatorFunc(&(*controller)->ram_unit,input);


    stat.dmem_error = result.status_flag & 1;
    if(!stat.dmem_error) {
        memcpy(stat.data.offset,result.data,32 * sizeof(uint32_t));
    }

    return stat;
}

RAM_RD_STATUS rd_memory_4b(PWOLF_MEM_CONTROLLER* controller,uint32_t paddr,uint8_t be) {
    RAM_RD_STATUS stat = {0};

    RAM_IN_ARGS input = {0};
    input.opcode = RAM_IN_OPCODE_RD;
    input.opbytes = RAM_IN_OPBYTES_4BYTE;
    input.paddr = paddr;

    RAM_OPERATOR_RESULT result = (*controller)->ram_unit->operatorFunc(&(*controller)->ram_unit,input);

    stat.dmem_error = result.status_flag & 1;
    if(!stat.dmem_error) {
        memcpy(stat.data.offset,result.data,BE_DATA(be));
    }
    return stat;
}

RAM_WR_STATUS wr_mem(PWOLF_MEM_CONTROLLER* controller,uint32_t paddr,uint8_t value[4],uint8_t be) {
    RAM_WR_STATUS stat = {0};

    RAM_IN_ARGS input = {0};
    input.opcode = RAM_IN_OPCODE_WR;
    input.opbytes = RAM_IN_OPBYTES_4BYTE;
    input.paddr = paddr;
    
    COPY_BYTE_4_ARRAY(input.val4bIn,value);

    RAM_OPERATOR_RESULT result = (*controller)->ram_unit->operatorFunc(&(*controller)->ram_unit,input);

    stat.dmem_error = result.status_flag & 1;
    if(!stat.dmem_error) {
        memcpy(&(stat.offset4),result.data,BE_DATA(be));
    }
    return stat; 
}

WOLF_MEM_CONTROLLER* init_mem_controller() {
    WOLF_MEM_CONTROLLER* controller = (WOLF_MEM_CONTROLLER*)calloc(1,sizeof(WOLF_MEM_CONTROLLER));
    if(controller == NULL)  goto MEM_ALLOC_FAIL;
    RAM_INTERFACE_UNIT* unit = init_ram_unit();
    if(unit == NULL) goto UNIT_ALLOC_FAIL;
    controller->ram_unit = unit;
    controller->rd_ram = rd_memory;
    controller->rd_ram_4b = rd_memory_4b;
    controller->wr_ram = wr_mem;
    return controller;
UNIT_ALLOC_FAIL: free(controller);
MEM_ALLOC_FAIL:   return NULL;
}

void free_mem(WOLF_MEM_CONTROLLER** controller) {
    if(*controller != NULL) {
        free(*controller);
        *controller = NULL;
    }
}