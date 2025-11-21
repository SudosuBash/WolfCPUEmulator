#include <tools/files.h>
#include <stdlib.h>


uint8_t* read_bin(const char* name,int maxreadCount) {
    FILE* fp = fopen(name,"rb");
    if(fp == NULL) {
        return NULL;
    }
    uint8_t* buf = (uint8_t*)calloc(maxreadCount,1);
    size_t rd_bytes = fread(buf,sizeof(uint8_t),maxreadCount,fp);
    if(ferror(fp)) {
        free(buf);
        return NULL;
    }
    return buf;
}
