#include <mmio_devices/device_init.h>
#include <pthread.h>
#include <stdio.h>
#include <mmio_devices/stdo_device.h>
#include <unistd.h>
#include <mmio_devices/irq_controller.h>
#include <mmio_devices/clock_device.h>

void* start_device_handle(void* pdevice) {
    PWOLF_CPU_BUS_DEVICE* device = (PWOLF_CPU_BUS_DEVICE*) pdevice;
    WOLF_CPU_BUS_DEVICE* dev = *device;
    while (1) {
        (*device)->start_func(device);
    }
    return NULL;
}

static inline void create_thread(PWOLF_CPU_BUS_DEVICE* device) {
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&thread,&attr,start_device_handle,device);
    usleep(1000);
    (*device)->thread_attr = attr;
    (*device)->thread = thread;
}
INIT_STATUS init_devices(WOLF_CPU* cpu) {
    WOLF_CPU_BUS_CONTROLLER* controller = cpu->bus;
    
    PWOLF_CPU_BUS_DEVICE* stdo_device = init_stdo_device(controller);
    WOLF_MMIO_STDO_DEVICE* dev = get_parent_struct(stdo_device,WOLF_MMIO_STDO_DEVICE,bus_device);
    if(stdo_device == NULL) goto INIT_DEVICE_FAIL;
    register_device(&controller,*stdo_device,0);
    create_thread(stdo_device);

    PWOLF_CPU_BUS_DEVICE* irq_device = init_irq_controller(controller,cpu);
    WOLF_IRQ_CONTROLLER* idev = get_parent_struct(irq_device,WOLF_IRQ_CONTROLLER,bus_device);
    if(irq_device == NULL) goto INIT_DEVICE_IRQ_FAIL;
    register_device(&controller,*irq_device,1);
    create_thread(irq_device);

    PWOLF_CPU_BUS_DEVICE* clock_device = init_clock_device(controller,idev);
    WOLF_MMIO_CLOCK_DEVICE* clock_dev = get_parent_struct(clock_device,WOLF_MMIO_CLOCK_DEVICE,bus_device);
    if(clock_device == NULL) goto INIT_DEVICE_CLOCK_FAIL;
    register_device(&controller,*clock_device,2);
    create_thread(clock_device);

    return DEVICE_INIT_STATUS_SUCCESS;

INIT_DEVICE_CLOCK_FAIL: destroy_irq_device(&idev);
INIT_DEVICE_IRQ_FAIL: destroy_stdo_device(&dev);
INIT_DEVICE_FAIL:
    return DEVICE_INIT_STATUS_ERROR;
}