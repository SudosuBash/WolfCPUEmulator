#include <mmio_devices/device_init.h>
#include <pthread.h>
#include <windows.h>

#include <mmio_devices/stdo_device.h>

void* start_device_handle(void* pdevice) {
    WOLF_CPU_BUS_DEVICE* device = (WOLF_CPU_BUS_DEVICE*) device;
    // while (1) {
    //     //device->start_func(&device);
    //     Sleep(1);
    // }
    return NULL;
}

static inline void create_thread(WOLF_CPU_BUS_DEVICE* device) {
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&thread,&attr,start_device_handle,device);
    device->thread_attr = attr;
    device->thread = thread;
}
INIT_STATUS init_devices(WOLF_CPU_BUS_CONTROLLER* controller) {
    WOLF_CPU_BUS_DEVICE* device = init_stdo_device(controller);
    if(device == NULL) goto INIT_DEVICE_FAIL;
    register_device(&controller,device,0);
    create_thread(device);

    return DEVICE_INIT_STATUS_SUCCESS;
INIT_DEVICE_FAIL:
    return DEVICE_INIT_STATUS_ERROR;
}