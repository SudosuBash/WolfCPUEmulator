#include <cpu.h>
static inline uint32_t get_reg_data(uint32_t origin,uint32_t mask,uint32_t data) {
    return (origin & ~mask) | (data & mask);
}
//没写完
void writeback(WOLF_CPU* cpu) {
    WCPUMemResult result = cpu->mem_data_reg;
   
    WCPUWBResult res;
    if(result.noexception == 0) {
        res.noexception = 0;
        goto WB_ERR_END;
        
    }
    uint8_t icode = result.icode;
    uint8_t cond = (icode == ICODE_MOV);
    
    switch(cond) {
        case ICODE_MOV: {
            uint32_t mask = BE_MASK_GEN(result.ExFlag >> 2);
            cpu->gen_regs.r[result.destReg] = get_reg_data(
                cpu->gen_regs.r[result.destReg],
                mask,
                result.valC
            );
            break;
        }
        case ICODE_ALU: {
            cpu->gen_regs.r[result.destReg] = get_reg_data(
                cpu->gen_regs.r[result.destReg],
                0b1111,
                result.valC
            );
            break;
        }
        case ICODE_RERE: {
            cpu->gen_regs.r[result.destReg] = result.valB;
            break;
        }
        case ICODE_RIRE: {
            cpu->gen_regs.r[result.destReg] = result.valB;
            break;
        }
    }
    
    res.icode = result.icode;
    res.valC = result.valC;
    res.noexception = result.noexception;
WB_ERR_END:
    cpu->wb_result_reg = res;
}