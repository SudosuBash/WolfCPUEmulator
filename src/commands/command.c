#include <commands/commands.h>
#include <string.h>
#include <stdio.h>

CMD_PARSER_MANAGER cmd_parser;
uint8_t trigger_command(CMD_PARSER_MANAGER* manager,char* cmd,char* arg) {
    CMD_PARSER_OBJECT obj = {.cmd = 0,.desc = 0,.op_func = NULL};
    for(int i=0;i<manager->cmdLen;i++) {
        if(!strcmp(manager->objects[i].cmd,cmd)) {
            obj = manager->objects[i];
            break;
        }
    }
    if(obj.op_func == NULL) {
        printf("E:Unexpected argument: %s",cmd);
        return -1;
    }
    return obj.op_func(arg);
}

void register_command(CMD_PARSER_MANAGER* manager,CMD_PARSER_OBJECT object) {
    manager->objects[manager->cmdLen] = object;
    manager->cmdLen++;
}

CMD_PARSER_MANAGER* init_command_system() {
    register_command(&cmd_parser, init_load_bios_command());
    register_command(&cmd_parser, init_help_command(&cmd_parser));
    register_command(&cmd_parser, init_load_rootfs_command());
    register_command(&cmd_parser, init_debug_command());
    return &cmd_parser; //省得自己回收
}

uint8_t exec_cmd(CMD_PARSER_MANAGER* manager, int argc,char** argv) {
    if(!(argc % 2)) {
        printf("WARNING: You have put an unrecognized command.Here is the help doc.\n");
        return trigger_command(manager,"-help","help");
    }
    uint8_t status = 0;
    for(int i=1;i<(argc+1)/2;i+=2) {
        status = trigger_command(manager,argv[i],argv[i+1]);
        if(status != 0) {
            printf("Emulator exited.");
            return status;
        }
    }
    return 0;
}