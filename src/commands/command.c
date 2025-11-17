#include <commands/commands.h>
#include <string.h>
#include <stdio.h>

CMD_PARSER_MANAGER cmd_parser;
uint8_t trigger_command(char* cmd,char* arg) {
    CMD_PARSER_OBJECT obj = {.cmd = 0,.desc = 0,.op_func = NULL};
    for(int i=0;i<cmd_parser.cmdLen;i++) {
        if(!strcmp(cmd_parser.objects[i].cmd,cmd)) {
            obj = cmd_parser.objects[i];
            break;
        }
    }
    if(obj.op_func == NULL) {
        printf("E:Unexpected argument: %s",cmd);
        return -1;
    }
    return obj.op_func(arg);
}

void register_command(CMD_PARSER_OBJECT object) {
    cmd_parser.objects[cmd_parser.cmdLen] = object;
    cmd_parser.cmdLen++;
}

void init_command_system() {
    register_command(init_load_bios_command());
    register_command(init_help_command(&cmd_parser));
    register_command(init_load_rootfs_command());
    register_command(init_debug_command());
}

uint8_t exec_cmd(int argc,char** argv) {
    if(!(argc % 2)) {
        printf("WARNING: You have put an unrecognized command.Here is the help doc.\n");
        return trigger_command("-help","help");
    }
    uint8_t status = 0;
    for(int i=0;i<argc/2;i++) {
        status = trigger_command(argv[i],argv[i+1]);
        if(status != 0)
            return status;
    }
    return 0;
}