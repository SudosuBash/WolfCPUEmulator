#include <cpu.h>
#include <tools/endian.h>
void memory(WOLF_CPU* cpu) {
    WOLF_CPU_MMU_CONTROLLER* controller = cpu->mmu;
    if(!cpu->temp_data_reg.noexception) 
        goto CPU_MEM_END_STATUS;
    uint8_t icode = cpu->temp_data_reg.icode;
    uint8_t exflag = cpu->temp_data_reg.ExFlag;
    
    uint8_t be = ICODE_EXFLAG_MOV_BE(exflag >> 3);
    uint32_t wr_data = Through32((
        icode == ICODE_PUSH
    ),cpu->temp_data_reg.valC) | Through32(
        icode == ICODE_OCALL
    ,cpu->temp_data_reg.valP);
    uint32_t wr_addr = Through32((
        icode == ICODE_OCALL ||
        icode == ICODE_PUSH
    ),cpu->temp_data_reg.valB);

    uint32_t rd_addr = Through32((
        icode == ICODE_RET
    ),cpu->temp_data_reg.valA) |
    Through32((
        icode == ICODE_POP
    ),cpu->temp_data_reg.valB - 4);
    switch(cpu->temp_data_reg.icode) {
        case ICODE_MOV: {
            MMU_STATUS stat = {.stat = 0,.data = 0};
            if(!IS_RTYPE(cpu->temp_data_reg.irtype)) break;
            uint32_t addr;
            if(ICODE_EXFLAG_MOV_MEM1(cpu->temp_data_reg.ExFlag)) {
                addr = Through32((
                        icode == ICODE_MOV &&
                        ICODE_EXFLAG_MOV_MEM1(cpu->temp_data_reg.ExFlag)
                ),cpu->temp_data_reg.valC);
                MMU_DATA data = {.data = {SEP_INT_FOR_4_BYTES_L(cpu->temp_data_reg.valB)},.be = be};
                stat = controller->wr_mmu(&cpu->mmu,addr,data);
            } else if(ICODE_EXFLAG_MOV_MEM2(cpu->temp_data_reg.ExFlag)) {
                addr = Through32((
                        icode == ICODE_MOV &&
                        ICODE_EXFLAG_MOV_MEM2(cpu->temp_data_reg.ExFlag)
                    ),cpu->temp_data_reg.valC);
                //大端转换成小端，获得的小端转回来
                stat =  controller->rd_mmu(&cpu->mmu,addr,be);
                cpu->temp_data_reg.valC = GET_INT_FROM_4_BYTES_L(stat.data);
            }
            if(stat.stat != BCR_RAM_ERR_OK) {
                cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,MMU_CONVERT_TO_EREASON(stat.stat),addr);
                cpu->temp_data_reg.noexception = 0;
                goto CPU_MEM_END_STATUS;
            }
            break;
        }
        case ICODE_RET: 
            if(cpu->temp_data_reg.ExFunc != ICODE_RET_EXFUNC) break; //确保普通的ret
            //M[valC] <- valA;
        case ICODE_POPF:
        case ICODE_POP: {
            MMU_STATUS stat = controller->rd_mmu(&cpu->mmu,rd_addr,be);
            if(stat.stat != BCR_RAM_ERR_OK) {
                cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,MMU_CONVERT_TO_EREASON(stat.stat),rd_addr);
                cpu->temp_data_reg.noexception = 0;
                goto CPU_MEM_END_STATUS;
            }
            cpu->temp_data_reg.valC = GET_INT_FROM_4_BYTES_L(stat.data);
            break;
        }
        case ICODE_PUSHF:
        case ICODE_PUSH:
        case ICODE_OCALL: {
            MMU_DATA data = {.data = {SEP_INT_FOR_4_BYTES_L(wr_data)},.be = be,.status = 0};
            MMU_STATUS stat = controller->wr_mmu(&cpu->mmu,wr_addr,data);
            if(stat.stat != BCR_RAM_ERR_OK) {
                cpu->ecall_controller->ecaller(&cpu->ecall_controller,ECALL_MACHINE_PROBLEM,MMU_CONVERT_TO_EREASON(stat.stat),wr_addr);
                cpu->temp_data_reg.noexception = 0;
                goto CPU_MEM_END_STATUS;
            }
            break;            
        }
    }
CPU_MEM_END_STATUS:
}