
#include <ram/ram_eff.h>
#include <stdlib.h>
#include <memory.h>
uint8_t get_data(uint8_t* ram_page_table[RAM_PDE_ITEM][RAM_PTE_ITEM],uint32_t addr) {
    uint32_t pde = addr >> RAM_PDE_POS;
    uint32_t pte = (addr >> RAM_PTE_POS) & RAM_PTE_MASK;
    uint32_t offset = (addr & RAM_OFFSET_MASK);
    if(ram_page_table[pde][pte] == NULL) {
        uint8_t* ram = (uint8_t*)calloc(RAM_SINGLE_BLK_SIZE, sizeof(uint8_t));
        memset(ram,0,RAM_SINGLE_BLK_SIZE * sizeof(uint8_t));
        ram_page_table[pde][pte] = ram; //分配内存
    }
    return ram_page_table[pde][pte][offset];
}


void write_data(uint8_t* ram_page_table[RAM_PDE_ITEM][RAM_PTE_ITEM],uint32_t addr,uint8_t data,uint8_t be) {
    uint32_t pde = addr >> RAM_PDE_POS;
    uint32_t pte = (addr >> RAM_PTE_POS) & RAM_PTE_MASK;
    uint32_t offset = (addr & RAM_OFFSET_MASK);
    if(ram_page_table[pde][pte] == NULL) {
        uint8_t* ram = (uint8_t*)calloc(RAM_SINGLE_BLK_SIZE, sizeof(uint8_t));
        memset(ram,0,RAM_SINGLE_BLK_SIZE * sizeof(uint8_t));
        ram_page_table[pde][pte] = ram; //分配内存
    }
    ram_page_table[pde][pte][offset] = data;
}

RAM_OPERATOR_RESULT operate(PRAM_INTERFACE_UNIT* punit,RAM_IN_ARGS input) {
    RAM_INTERFACE_UNIT* unit = *punit;
    //这儿没考虑对齐问题，原因很简单，不对齐的话第一轮访问缓存就被刷下来了
    RAM_OPERATOR_RESULT result = {0};
    uint32_t addr = (input.opbytes == RAM_IN_OPBYTES_128BYTE && input.opcode== RAM_IN_OPCODE_RD)? (input.paddr & ~(L2_OFFSET_MASK)) : input.paddr;
    uint32_t op_size = input.opbytes == RAM_IN_OPBYTES_128BYTE ? 32 :4;

    if(addr+op_size >= RAM_SIZE) {
        result.status_flag = 1;
    }
    switch(input.opcode) {
        case RAM_IN_OPCODE_RD:
            for(uint8_t i = 0;i<op_size;i++) {
                result.data[i] = get_data(unit->ram_page_table,addr+i);
            }
            break;
        case RAM_IN_OPCODE_WR:
            write_data(unit->ram_page_table,addr,input.val4bIn[0],GET_DATA_BIT0(input.be));
            write_data(unit->ram_page_table,addr+1,input.val4bIn[1],GET_DATA_BIT1(input.be));
            write_data(unit->ram_page_table,addr+2,input.val4bIn[2],GET_DATA_BIT2(input.be));
            write_data(unit->ram_page_table,addr+3,input.val4bIn[3],GET_DATA_BIT3(input.be));
            break;
    }
    return result;
}

RAM_INTERFACE_UNIT* init_ram_unit() {
    RAM_INTERFACE_UNIT* unit = (RAM_INTERFACE_UNIT*) calloc(1,sizeof(RAM_INTERFACE_UNIT));
    unit->operatorFunc = operate;
    return unit;
}
void free_ram_unit(RAM_INTERFACE_UNIT* unit) {
    if(unit != NULL) {
        for(int i=0;i<RAM_PDE_ITEM;i++) {
            for(int j=0;j<RAM_PTE_ITEM;j++) {
                if(unit->ram_page_table[i][j] != NULL) {
                    free(unit->ram_page_table[i][j]);
                    unit->ram_page_table[i][j] = NULL;
                }
            }
        }
        free(unit);
        unit = NULL;
    }
}