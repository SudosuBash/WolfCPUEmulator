#ifndef _WOLF_CPU_BIOS
#define _WOLF_CPU_BIOS

#include <stdint.h>
typedef struct {
    uint64_t size;
    uint8_t* file;
} BIOS_FILE;

void load_bios();
void free_bios();
#endif