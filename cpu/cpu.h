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
#include "alu.h"
#include "cpu_fetch.h"
#include "cpu_decode.h"
#include "cpu_execute.h"

#define IS_IN_KERN_MODE(cpu) \
        ((cpu)->spe_regs.bcr & KERN_MODE_MASK)

#define IS_CACHE_ON(cpu) \
    ((cpu)->spe_regs.bcr & CACHE_OPEN_MASK) >> 1
#define IS_PGO_ON(cpu) \
    ((cpu)->spe_regs.bcr & BCR_PGO_MASK) >> 2

#define BASE_MMIO_ADDR 0xff000000
typedef struct {
    uint32_t pc;
    
    CPU_General_Registers gen_regs;
    CPU_Special_Registers spe_regs;
    CPU_Ecall_Registers ecall_regs;
    CPU_Ecall_Registers irq_regs;
    WOLF_ALU* alu;

    WOLF_CPU_ECALL_CONTROLLER* ecall_controller;
    WOLF_MEM_CONTROLLER* mem_controller;
    WOLF_CACHE_CONTROLLER* cache_controller;
    WOLF_CPU_BUS_CONTROLLER* bus;
    WOLF_CPU_MMU_CONTROLLER* mmu;

    MACHINE_L1_CACHE_GROUP** cache1;
    MACHINE_L2_CACHE_GROUP** cache2;
    

    WCPUFetchData if_data_reg;
    WCPUDecodedData id_data_reg;
    WCPUExecuteResult ex_data_reg;
    WCPUMemResult mem_data_reg;
    
    pthread_mutex_t clock_execution; //CPU 时序锁
    //真实电路都是按照时序来的，不需要这玩意，这个只是模拟时序
} WOLF_CPU;

// typedef void (*ecall)(WOLF_CPU* cpu,uint8_t ecode);

void start_cpu(WOLF_CPU* cpu);

void init_env();
WOLF_CPU* init_cpu();
#endif