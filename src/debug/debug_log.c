#include <debug/debug_io.h>
uint32_t error_log(const char* str) {
    return printf("%s",str);
}

uint32_t debug_log(const char* str) {
#if DEBUG_ON
    return printf("%s",str);
#endif
    return 0;
}