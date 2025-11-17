#include <debug/debug.h>
#include <cpu.h>
#include <stdio.h>

static void print_reg_info(WOLF_CPU* cpu) {
    printf("========== REGISTERS ==========\n");
    printf("R1: 0x%08x,R2: 0x%08x,R3: 0x%08x,R4: 0x%08x,R5: 0x%08x\n",
            cpu->gen_regs.r[0],
            cpu->gen_regs.r[1],
            cpu->gen_regs.r[2],
            cpu->gen_regs.r[3],
            cpu->gen_regs.r[4]);
    printf("R6: 0x%08x,R7: 0x%08x,R8: 0x%08x,R9: 0x%08x,R10: 0x%08x\n",
        cpu->gen_regs.r[5],
        cpu->gen_regs.r[6],
        cpu->gen_regs.r[7],
        cpu->gen_regs.r[8],
        cpu->gen_regs.r[9]);
    printf("R11: 0x%08x,R12: 0x%08x,R13: 0x%08x,R14: 0x%08x,R15(RSP): 0x%08x\n",
        cpu->gen_regs.r[10],
        cpu->gen_regs.r[11],
        cpu->gen_regs.r[12],
        cpu->gen_regs.r[13],
        cpu->gen_regs.r[14]);
    printf("PC: 0x%08x\n",
        cpu->pc);
    printf("BCR: %d\n",
        cpu->spe_regs.bcr);
    printf("SCR: %s, %s, %s, %s, %s\n",
        GET_SCR_CF_FLAG(cpu->spe_regs.scr) ? "CF" : "NC",
        GET_SCR_SF_FLAG(cpu->spe_regs.scr) ? "SF" : "NS",
        GET_SCR_ZF_FLAG(cpu->spe_regs.scr) ? "ZF" : "NZ",
        GET_SCR_PF_FLAG(cpu->spe_regs.scr) ? "PF" : "NP",
        GET_SCR_OF_FLAG(cpu->spe_regs.scr) ? "OF" : "NO");
}
static void print_current_exec_cmd(WOLF_CPU* cpu) {
    printf("Current Execution Command(ByteCode): %s %s %s %s");
}
uint32_t break_execution(WOLF_CPU* cpu) {
    printf("Break Signal triggered.\n");
    print_reg_info(cpu);
    getchar();
    return 0;
}