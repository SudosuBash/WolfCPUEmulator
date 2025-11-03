#include "bus.h"

static void wait_for_delta() {
    uint16_t val  = 30000;
    while(val>0) val-=1;
}