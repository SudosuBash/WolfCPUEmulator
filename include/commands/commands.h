#ifndef _CMD_PARSER_H
#define _CMD_PARSER_H
#include <stdint.h>
#include <stdio.h>
#include <bios_loader/bios.h>
typedef uint8_t (*cmd_exec)(char* arg);
typedef struct {
    char cmd[30];
    char desc[512];
    cmd_exec op_func;
} CMD_PARSER_OBJECT;

typedef struct {
    CMD_PARSER_OBJECT objects[101];
    uint8_t cmdLen;
} CMD_PARSER_MANAGER;

typedef struct {
    uint8_t debug;
    BIOS_FILE bios;
} CMD_CONFIGURATIONS;

void register_command(CMD_PARSER_MANAGER* manager,CMD_PARSER_OBJECT object);
uint8_t trigger_command(CMD_PARSER_MANAGER* manager,char* cmd,char* arg);
CMD_PARSER_MANAGER* init_command_system();
CMD_PARSER_OBJECT init_load_bios_command();
CMD_PARSER_OBJECT init_help_command(CMD_PARSER_MANAGER* manager);
CMD_PARSER_OBJECT init_load_rootfs_command();
CMD_PARSER_OBJECT init_debug_command();
uint8_t exec_cmd(CMD_PARSER_MANAGER* manager, int argc,char** argv);
#endif