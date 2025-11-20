#include <bios_loader/bios.h>
#include <stdio.h>
#include <stdlib.h>
BIOS_FILE bf;

void load_bios() {
    if(bf.file == NULL) {
        bf.file = (uint8_t*)calloc(sizeof(uint8_t*) * 1024,1);
        printf("WARNING: BIOS File is not loaded.\n");
    }
}

void free_bios() {
    if(bf.file != NULL) {
        free(bf.file);
    }
}