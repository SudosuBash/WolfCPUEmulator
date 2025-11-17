#include <commands/commands.h>

static CMD_PARSER_MANAGER* cmd_manager;
static uint8_t cmd_execer(char* args) {
    printf("=========HELP OF WOLF CPU EMULATOR=========\n");
    printf("Options of this emulator:\n");
    for(int i=0;i<cmd_manager->cmdLen;i++) {
        printf(" %s: %s\n",cmd_manager->objects[i].cmd,cmd_manager->objects[i].desc);
    }
    return 1;
}
CMD_PARSER_OBJECT init_help_command(CMD_PARSER_MANAGER* manager) {
    CMD_PARSER_OBJECT obj = {
        .cmd = "-help",
        .desc = "Usage: -help help. Read the help docs.",
        .op_func = cmd_execer,
    };
    cmd_manager = manager;
    return obj;
}