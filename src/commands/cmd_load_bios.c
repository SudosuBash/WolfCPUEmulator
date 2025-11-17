
#include <commands/commands.h>


static uint8_t cmd_execer(char* args) {
    return 0;
}
CMD_PARSER_OBJECT init_load_bios_command() {
    CMD_PARSER_OBJECT obj = {
        .cmd = "-lb",
        .desc = "Usage: -lb <bios_file>, to load the BIOS bin.",
        .op_func = cmd_execer
    };
    return obj;
}