#ifndef __WOLF_CPU
#define __WOLF_CPU
#include "controllers/bus.h"
#include "register.h"
#include "controllers/mmu_controller.h"
#include "controllers/ecall.h"
#include "controllers/cache.h"
#include "controllers/cache_controller.h"
#include "controllers/mem_controller.h"
#include <pthread.h>
#include <stdint.h>

#define IS_IN_KERN_MODE(cpu) \
        (cpu->spe_regs.bcr & KERN_MODE_MASK)


typedef struct {
    uint32_t pc;
    CPU_General_Registers gen_regs;
    CPU_Special_Registers spe_regs;
    CPU_Ecall_Registers ecall_regs;
    
    WOLF_CPU_ECALL_CONTROLLER* ecall_controller;
    WOLF_MEM_CONTROLLER* mem_controller;
    WOLF_CACHE_CONTROLLER* cache_controller;
    WOLF_CPU_BUS_CONTROLLER* bus;

    MACHINE_L1_CACHE_GROUP* cache1[CACHE_L1_GROUPS];
    MACHINE_L2_CACHE_GROUP* cache2[CACHE_L2_GROUPS];
    
    pthread_mutex_t clock_execution; //CPU 时序锁
    //真实电路都是按照时序来的，不需要这玩意，这个只是模拟时序
} WOLF_CPU;


// typedef void (*ecall)(WOLF_CPU* cpu,uint8_t ecode);

void start_cpu(WOLF_CPU* cpu);

void init_env();
#endif