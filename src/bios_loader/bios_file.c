#include <bios_loader/bios.h>
#include <stdio.h>
#include <stdlib.h>

#include <commands/commands.h>
extern CMD_CONFIGURATIONS config;

void load_bios() {
    if(config.bios.file == NULL) {
        config.bios.file = (uint8_t*)calloc(sizeof(uint8_t*) * 1024,1);
        printf("[WARNING] BIOS File is not loaded.\n");
    }
}

void free_bios() {
    if(config.bios.file != NULL) {
        free(config.bios.file);
    }
}