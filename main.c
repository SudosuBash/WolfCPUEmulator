#include <stdio.h>
#include <cpu.h>
#include <bios_loader/bios.h>
#include <commands/commands.h>
int rd_bios(char* filec,BIOS_FILE* bf) {
    BIOS_FILE f;
    FILE* fp = fopen(filec,"rb");
    if(fp == NULL) {
        printf("Sorry,Load BIOS Failed, Status = %d\n",errno);
        return -1;
    }
    return 0;
}
int main(int argc,char* argv[]) {

    init_command_system();
    if(exec_cmd(argc,argv) == 0) {
        printf("Welcome to Wolf CPU Emulator.\n");
        printf("Now starting CPU....\n");
        init_env();
    }
    return 0;
}