#include <debug/debug.h>
#include <cpu.h>
#include <stdio.h>
#include <commands/commands.h>

static BREAKPOINT_CONTROLLER controller;
static CMD_PARSER_MANAGER cmd_manager;

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
    printf("PC: 0x%08x    %s  %s  %s  %s  %s  %s  %s\n",
        cpu->pc,
        IS_IN_KERN_MODE(cpu) ? "KR" : "UR",
        IS_PGO_ON(cpu) ? "PG": "NG",
        IS_IRQ_ALLOW(cpu) ? "IR" : "NI",
        GET_SCR_CF_FLAG(cpu->spe_regs.scr) ? "CF" : "NC",
        GET_SCR_SF_FLAG(cpu->spe_regs.scr) ? "SF" : "NS",
        GET_SCR_ZF_FLAG(cpu->spe_regs.scr) ? "ZF" : "NZ",
        GET_SCR_PF_FLAG(cpu->spe_regs.scr) ? "PF" : "NP",
        GET_SCR_OF_FLAG(cpu->spe_regs.scr) ? "OF" : "NO");
}

static void breakpointSet(BREAKPOINT_CONTROLLER* controller,uint32_t pc) {
    uint32_t index = pc & 32767;
    if(controller->hashtable[index].break_pc == pc) {
        printf("WARNING: Breakpoint at 0x%08x already exists.\n",pc);
        return;
    }
    HASHTABLE_ELEMENT* next_ele = controller->hashtable[index].next;
    if(next_ele == NULL) {
        HASHTABLE_ELEMENT* target_ele = (HASHTABLE_ELEMENT*)calloc(1,sizeof(HASHTABLE_ELEMENT));
        controller->hashtable[index].next = target_ele;
        target_ele->break_pc = pc;
        target_ele->next = NULL;
    } else {
        while(next_ele->next != NULL) {
            if(next_ele->break_pc == pc) {
                printf("WARNING: Breakpoint at 0x%08x already exists.\n",pc);
                return;
            }
            next_ele = next_ele->next;
        }
        HASHTABLE_ELEMENT* target_ele = (HASHTABLE_ELEMENT*)calloc(1,sizeof(HASHTABLE_ELEMENT));
        next_ele->next = target_ele;
        target_ele->break_pc = pc;
        target_ele->next = NULL;
    }
    printf("Completed to set debug address: 0x%08x\n",pc);
}
static HASHTABLE_ELEMENT* breakpointGet(BREAKPOINT_CONTROLLER* controller,WOLF_CPU* cpu,uint32_t pc) {
    uint32_t index = pc & 32767;
    if(controller->hashtable[index].break_pc == pc) {
        return &controller->hashtable[index];
    }
    HASHTABLE_ELEMENT* next_ele = controller->hashtable[index].next;
    while(next_ele != NULL) {
        if(next_ele->break_pc == pc) {
            return next_ele;
        }
        next_ele = next_ele->next;
    }
    return NULL;
}


static uint8_t exec_debug_cmd(CMD_PARSER_MANAGER* manager, int argc,char** argv) {
    if(argc != 2) {
        printf("WARNING: You have put an unrecognized command.Here is the help doc.\n");
        return trigger_command(manager,"h","help");
    }
    return trigger_command(manager,argv[0],argv[1]);
}

static uint8_t bp_cmd_execer(char* args) {
    uint64_t addr = strtoul(args,NULL,16);
    int err = errno;
    if(errno != 0) {
        printf("E: Breakpoint Error: Invalid address: %s\n", args);
        return 1;
    }
    if(addr & 3) {
        printf("E: Breakpoint Error: Address must be 4-bytes aligned: %s\n",args);
        return 2;
    }
    controller.set_breakpoint(&controller, (uint32_t)addr);
    return 0;
}
static CMD_PARSER_OBJECT bp_cmd(CMD_PARSER_MANAGER* manager) {
    CMD_PARSER_OBJECT obj = {
        .cmd = "b",
        .desc = "Usage: b <addr>, add a breakpoint.",
        .op_func = bp_cmd_execer,
    };
    return obj;
}

static void init_debug_cmd_system() {
    register_command(&cmd_manager, bp_cmd(&cmd_manager));
}

static uint8_t parse_cmd(char* cmd) {
    char* arg = strtok(cmd," ");
    char* args[101] = {0};
    int strLen = 0;
    while (arg!=NULL && strLen < 101)
    {
        args[strLen] = arg;
        strLen++;
        arg = strtok(NULL, " ");
    }
    return exec_debug_cmd(&cmd_manager,strLen,args);
}

static uint8_t input_cmd() {
    char cmd[1001] = {0};
    uint16_t strLen = 0;
    while(strLen < 1001) {
        char c = getchar();
        if(c=='\n') break;

        cmd[strLen] = c;
        strLen++;
    }
    if(cmd[0] == 'c') {
        return DEBUG_CMD_STATUS_CONTINUE;
    }
    return parse_cmd(cmd);
}
uint32_t break_execution(WOLF_CPU* cpu) {
    HASHTABLE_ELEMENT* elem = controller.get_breakpoint(&controller,cpu,cpu->pc);
    if(elem == NULL) {
        return 0;
    }
    printf("\nBreak Signal triggered at 0x%08x\n", cpu->pc);
    print_reg_info(cpu);
    printf("Enter command, type 'c' to continue: ");
    while(input_cmd() != DEBUG_CMD_STATUS_CONTINUE) {
        printf("\n");
        printf("Enter command, type 'c' to continue: ");
    };
    return 0;
}

void init_break_execution() {
    controller.set_breakpoint = breakpointSet;
    controller.get_breakpoint = breakpointGet;

    init_debug_cmd_system();
    controller.set_breakpoint(&controller,0xfffffe00); //机器执行第一条指令时中断
}

void free_break_execution() {
    for(int i=0;i<32768;i++) {
        HASHTABLE_ELEMENT* ele = controller.hashtable[i].next;
        while(ele != NULL) {
            HASHTABLE_ELEMENT* next_ele = ele->next;
            free(ele);
            ele = next_ele;
        }
        controller.hashtable[i].next = NULL;
    }
}

