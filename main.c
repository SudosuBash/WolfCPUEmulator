#include <stdio.h>
#include <cpu.h>
#include <bios_loader/bios.h>
#include <commands/commands.h>
int main(int argc,char* argv[]) {
    CMD_PARSER_MANAGER* cmd_manager = init_command_system();
    if(exec_cmd(cmd_manager, argc, argv) == 0) {
        printf("Welcome to Wolf CPU Emulator.\n");
        printf("Now starting CPU....\n");
        init_env();
    }
    return 0;
}