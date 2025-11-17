#include <commands/commands.h>

static uint8_t cmd_execer(char* args) {
    
    return 0;
}
CMD_PARSER_OBJECT init_debug_command() {
    CMD_PARSER_OBJECT obj = {
        .cmd = "-dbg",
        .desc = "Usage: -dbg <true/false> .Came into debug mode.",
        .op_func = cmd_execer
    };
    return obj;
}