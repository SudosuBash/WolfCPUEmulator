#include <commands/commands.h>

static uint8_t cmd_execer(char* args) {
    
    return 0;
}
CMD_PARSER_OBJECT init_load_rootfs_command() {
    CMD_PARSER_OBJECT obj = {
        .cmd = "-lr",
        .desc = "Usage: Usage: -lr <rootfs_file>,to load the ROOTFS.",
        .op_func = cmd_execer
    };
    return obj;
}