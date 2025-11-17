#include <mmio_devices/device_init.h>
#include <pthread.h>
#include <stdio.h>
#include <windows.h>
#include <mmio_devices/stdo_device.h>
#include <unistd.h>
void* start_device_handle(void* pdevice) {
    PWOLF_CPU_BUS_DEVICE* device = (PWOLF_CPU_BUS_DEVICE*) pdevice;
    WOLF_CPU_BUS_DEVICE* dev = *device;
    while (1) {
        (*device)->start_func(device);
        usleep(20);
    }
    return NULL;
}

static inline void create_thread(PWOLF_CPU_BUS_DEVICE* device) {
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&thread,&attr,start_device_handle,device);
    Sleep(1);
    (*device)->thread_attr = attr;
    (*device)->thread = thread;
}
INIT_STATUS init_devices(WOLF_CPU_BUS_CONTROLLER* controller) {
    PWOLF_CPU_BUS_DEVICE* device = init_stdo_device(controller);
    WOLF_MMIO_STDO_DEVICE* dev = get_parent_struct(device,WOLF_MMIO_STDO_DEVICE,bus_device);
    
    if(device == NULL) goto INIT_DEVICE_FAIL;
    register_device(&controller,*device,0);
    create_thread(device);

    return DEVICE_INIT_STATUS_SUCCESS;
INIT_DEVICE_FAIL:
    return DEVICE_INIT_STATUS_ERROR;
}