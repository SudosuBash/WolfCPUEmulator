#ifndef __WOLF_CPU
#define __WOLF_CPU
#include <controllers/bus.h>
#include <register.h>
#include <controllers/mmu_controller.h>
#include <controllers/ecall.h>
#include <controllers/cache.h>
#include <controllers/cache_controller.h>
#include <controllers/mem_controller.h>
#include <global.h>
#include <pthread.h>
#include <stdint.h>
#include <logics/logic_alg.h>
#include "alu.h"

#define IS_IN_KERN_MODE(cpu) \
        ((cpu)->spe_regs.bcr & KERN_MODE_MASK)

#define IS_CACHE_ON(cpu) \
    ((cpu)->spe_regs.bcr & CACHE_OPEN_MASK) >> 1
#define IS_PGO_ON(cpu) \
    ((cpu)->spe_regs.bcr & BCR_PGO_MASK) >> 2

#define BASE_MMIO_ADDR 0xff000000
#define BASE_BIOS_ADDR 0xfffffe00
//BIOS 512 MiB

#define CPU_REG_SPE_BCR 0b10000
#define CPU_REG_SPE_SCR 0b10001
#define CPU_REG_SPE_FLAGS 0b10010
#define CPU_REG_SPE_PGBASE 0b10011
#define CPU_REG_ECALL_ECBASE 0b10100
#define CPU_REG_ECALL_ICBASE 0b10101

#define CPU_REG_RSP 13


#define ICODE_MOV 0b000001
#define ICODE_ALU 0b000010
#define ICODE_MLMR 0b000011
#define ICODE_JMP_I 0b100000
#define ICODE_JMP_II 0b100001
#define ICODE_ECALL 0b100010
#define ICODE_OCALL 0b100011
#define ICODE_RSCR 0b110001
#define ICODE_LSCR 0b110010
#define ICODE_LPGR 0b110011
#define ICODE_LIBR 0b110100
#define ICODE_LEBR 0b110101
#define ICODE_RET 0b100100

#define ICODE_RET_EXFUNC 0x01
#define ICODE_IRET_EXFUNC 0x10
#define ICODE_ERET_EXFUNC 0x11


#define CMD_ITYPE 0
#define CMD_RTYPE 1

#define ALU_FUN_CODE_ADD 0x1
#define ALU_FUN_CODE_MUL 0x2
#define ALU_FUN_CODE_AND 0x3
#define ALU_FUN_CODE_OR 0x4
#define ALU_FUN_CODE_XOR 0x5
#define ALU_FUN_CODE_NEG 0x6
#define ALU_FUN_CODE_SUB 0b1001
#define ALU_FUN_CODE_DIV 0b1010

#define ALU_EXFUNC_BITS 3

#define ALU_EXFUNC_NEG_MASK 0b1
#define ALU_EXFUNC_SGN_MASK 0b10

#define ALU_EXFUNC_MLMR_MASK 0x1
#define ALU_EXFUNC_ML 0x0
#define ALU_EXFUNC_MR 0x1

typedef struct {
    uint8_t irtype:1;
    uint8_t icode:6;
    uint8_t reg1:5;
    uint8_t reg2:5;
    uint16_t idata;
    uint8_t aluExFunc:3;
    uint8_t jmpExCond:4;
    uint8_t ExFlag:5;
    uint8_t noexception:1;//指令应该继续执行吗?
} WCPUFetchData;

typedef struct {
    uint8_t icode;
    uint8_t destReg;
    uint32_t valC_Extended;//用于乘除法
    uint32_t valC;
    uint32_t valB;
    uint8_t ExCond:3;
    uint8_t ExFunc:5;
    uint8_t ExFlag:5;
    uint8_t noexception:1;//指令应该继续执行吗?
} WCPUMemResult;

typedef struct {
    uint32_t valC;
    uint32_t valC_Extended;//用于乘除法
    uint32_t valB;
    uint32_t valA;
    uint8_t icode;
    uint8_t destReg;
    uint8_t ExCond:3;
    uint8_t ExFunc:5;
    uint8_t ExFlag:5;
    uint8_t noexception:1;//指令应该继续执行吗?
}WCPUExecuteResult;

typedef struct {
    uint8_t icode;
    uint8_t destRegs;
    uint32_t valA;
    uint32_t valB;
    uint32_t valC;
    uint8_t ExCond:4;
    uint8_t ExFunc:5;
    uint8_t ExFlag:5;
    uint8_t noexception:1;//指令应该继续执行吗?
} WCPUDecodedData;

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
typedef struct {
    
} WCPUWBResult;
WCPUExecuteResult execute(WOLF_CPU* cpu);
void start_cpu(WOLF_CPU* cpu);
WCPUFetchData fetchData(WOLF_CPU* cpu);
WCPUMemResult memory(WOLF_CPU* cpu);
WCPUDecodedData decode(WOLF_CPU* cpu);
void free_cpu(WOLF_CPU** cpu);

void init_env();
WOLF_CPU* init_cpu();
#endif