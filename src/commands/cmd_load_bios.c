
#include <commands/commands.h>
#include <tools/files.h>
#include <bios_loader/bios.h>

extern BIOS_FILE bf;
static uint8_t cmd_execer(char* args) {
    uint8_t* buf = read_bin(args,1024);
    if(buf == NULL) {
        perror("E: Cannot load BIOS File,because of");
        return -1;
    }
    BIOS_FILE file;
    file.size = 1024;
    file.file = buf;
    bf = file;
    printf("[INFO] Successfully loaded BIOS Program.\n");
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