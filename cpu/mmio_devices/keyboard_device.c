#include "keyboard_device.h"
#include <cpu/global.h>
static WOLF_MMIO_KEYBOARD_DEVICE* kb_device;

void device_start(WOLF_CPU_BUS_DEVICE* device) {
    WOLF_MMIO_KEYBOARD_DEVICE* kb=(WOLF_MMIO_KEYBOARD_DEVICE*)get_parent_struct(device,WOLF_MMIO_KEYBOARD_DEVICE,bus_device);
    
}