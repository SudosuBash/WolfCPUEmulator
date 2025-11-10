#ifndef _WOLF_CPU_BIOS
#define _WOLF_CPU_BIOS

#include <stdint.h>
typedef struct {
    uint64_t size;
    uint8_t* file;
    FILE* fp;
} BIOS_FILE;
#endif