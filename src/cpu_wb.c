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
    
    switch(icode) {
        case ICODE_MOV: {
            if(IS_RTYPE(result.irtype) && ICODE_EXFLAG_MOV_MEM1(result.ExFlag)) {
                break;
            }
            uint32_t mask = BE_MASK_GEN(result.ExFlag >> 3);
            write_reg_val(cpu,result.destReg, get_reg_data(
                cpu->gen_regs.r[result.destReg],
                mask,
                result.valC
            ));
            break;
        }
        case ICODE_ALU: {
            write_reg_val(cpu,result.destReg,get_reg_data(
                cpu->gen_regs.r[result.destReg],
                0b1111,
                result.valC
            ));
            break;
        }
        case ICODE_LEBR:
        case ICODE_LEPV:
        case ICODE_LIBR:
        case ICODE_LIPV:
        case ICODE_LPGR:
        case ICODE_LBCR:
            write_reg_val(cpu,result.destReg, result.valB);
            break;
        //4个指令全部传送valB
        case ICODE_RERE: 
        case ICODE_RIRE:
        case ICODE_ECALL:
        case ICODE_OCALL:
        case ICODE_PUSH:
        case ICODE_ZWC:
            cpu->gen_regs.r[result.destReg] = result.valB;
            break;
        case ICODE_POP:
            cpu->gen_regs.r[result.destReg2] = result.valB;
            cpu->gen_regs.r[result.destReg] = result.valC;
            break;
    }
    
    res.icode = result.icode;
    res.valC = result.valC;
    res.noexception = result.noexception;
    res.ExFunc = result.ExFunc;
    res.irtype = result.irtype;
WB_ERR_END:
    cpu->wb_result_reg = res;
}