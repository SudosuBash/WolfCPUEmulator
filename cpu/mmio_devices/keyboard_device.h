#ifndef __WOLF_KEYBOARD_DEVICE_H
#define __WOLF_KEYBOARD_DEVICE_H

#include <stdint.h>
#include <cpu/controllers/bus.h>
#include <pthread.h>

#define KEYBOARD_DEVICE_FUNC_REG 0x0
#define KEYBOARD_DEVICE_STAT_REG 0x1
#define KEYBOARD_DEVICE_WRITE_BUF 0x2
#define KEYBOARD_DEVICE_REGS 7
#define BUS_KEYBOARD_DEVICE_BASE_ADDR 0xF0000100

static const char device_name[] = "Wolf Virtual Keyboard";
static const char vendor_name[] = "Wolf Emulator";
static const uint8_t need_space = KEYBOARD_DEVICE_REGS;
static const uint16_t device_id = 0x1002;

typedef struct {
    WOLF_CPU_BUS_DEVICE* desc;
    uint8_t regs[KEYBOARD_DEVICE_REGS];
    pthread_rwlock_t device_rwlock;
} WOLF_MMIO_KEYBOARD_DEVICE;

#endif