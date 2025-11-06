#include "cpu_decode.h"
#include "cpu.h"

WCPUDecodedData decode(WOLF_CPU* cpu) {
    WCPUFetchData data = cpu->if_data_reg;

    WCPUDecodedData res = {0};
    if(!data.noexception) goto CPU_DECODE_END_STATUS;
    uint8_t reg1 = data.reg1;
    uint8_t reg2 = data.reg2;
    uint32_t val1 = Through32(reg1 != 0 && reg1 < MAX_GEN_REGISTER_COUNT,cpu->gen_regs.r[reg1-1]);
    uint32_t val2 = Through32(reg2 != 0 && reg2 < MAX_GEN_REGISTER_COUNT,cpu->gen_regs.r[reg2-1]);
    res.ExCond = data.jmpExCond;
    res.destRegs = reg1;
    res.icode = data.icode;
    res.ExFunc = data.aluExFunc;
    res.valA = val1;
    res.valB = val2;
    res.valC = data.idata;
    res.ExFlag = data.ExFlag;
    res.noexception = data.noexception; //上传
    if(val1 >= MAX_GEN_REGISTER_COUNT || val2 >= MAX_GEN_REGISTER_COUNT) {
        ecall(cpu->ecall_controller,ECALL_MACHINE_PROBLEM,EREASON_FOR_UNRECOGNIZED_REG);
        res.noexception = 0;    
    }
CPU_DECODE_END_STATUS:
    cpu->id_data_reg = res;
    return res;
}