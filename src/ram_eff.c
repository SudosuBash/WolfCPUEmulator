
#include <ram/ram_eff.h>
#include <stdlib.h>
#include <memory.h>
uint32_t get_data(uint32_t* ram_page_table[RAM_PDE_ITEM][RAM_PTE_ITEM],uint32_t addr) {
    uint32_t pde = addr >> 20;
    uint32_t pte = (addr >> 12) & 0xff;
    uint32_t offset = (addr & 0xfff);
    if(ram_page_table[pde][pte] == NULL) {
        uint32_t* ram = (uint32_t*)malloc(RAM_SINGLE_BLK_SIZE * sizeof(uint8_t));
        memset(ram,0,RAM_SINGLE_BLK_SIZE * sizeof(uint8_t));
        ram_page_table[pde][pte] = ram; //分配内存
    }
    return ram_page_table[pde][pte][offset];
}

void write_data(uint32_t* ram_page_table[RAM_PDE_ITEM][RAM_PTE_ITEM],uint32_t addr,uint32_t data) {
    uint32_t pde = addr >> 20;
    uint32_t pte = (addr >> 12) & 0xff;
    uint32_t offset = (addr & 0xfff);
    if(ram_page_table[pde][pte] == NULL) {
        uint32_t* ram = (uint32_t*)malloc(RAM_SINGLE_BLK_SIZE * sizeof(uint8_t));
        memset(ram,0,RAM_SINGLE_BLK_SIZE * sizeof(uint8_t));
        ram_page_table[pde][pte] = ram; //分配内存
    }
    ram_page_table[pde][pte][offset] = data;
}

RAM_OPERATOR_RESULT operate(RAM_INTERFACE_UNIT* unit,RAM_IN_ARGS input) {
    //这儿没考虑对齐问题，原因很简单，不对齐的话第一轮访问缓存就被刷下来了
    RAM_OPERATOR_RESULT result = {0};
    uint32_t addr = input.paddr;
    uint32_t op_size = input.opbytes ? 32 : 1;
    if(input.opcode == RAM_IN_OPCODE_WR) op_size = 1;

    if(addr+op_size >= RAM_SIZE) {
        result.status_flag = 1;
    }
    switch(input.opcode) {
        case RAM_IN_OPCODE_RD:
            for(uint8_t i = 0;i<op_size;i++) {
                result.data[i] = get_data(unit->ram_page_table,addr);
            }
        case RAM_IN_OPCODE_WR:
            write_data(unit->ram_page_table,addr,input.val4bIn);
    }
    return result;
}

RAM_INTERFACE_UNIT* init_ram_unit() {
    RAM_INTERFACE_UNIT* unit = (RAM_INTERFACE_UNIT*) malloc(sizeof(RAM_INTERFACE_UNIT));
    memset(unit,0,sizeof(unit));
    return unit;
}
void free_ram_unit(RAM_INTERFACE_UNIT* unit) {

}