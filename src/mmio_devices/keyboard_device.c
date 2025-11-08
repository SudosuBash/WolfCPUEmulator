#include <mmio_devices/keyboard_device.h>
#include <global.h>
static WOLF_MMIO_KEYBOARD_DEVICE* kb_device;

static void device_start(PWOLF_CPU_BUS_DEVICE* pdevice) {
    WOLF_MMIO_KEYBOARD_DEVICE* kb=get_parent_struct(pdevice,WOLF_MMIO_KEYBOARD_DEVICE,bus_device);
    
}