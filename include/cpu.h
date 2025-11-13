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
        !((cpu)->spe_regs.bcr & KERN_MODE_MASK)

#define IS_CACHE_ON(cpu) \
    ((cpu)->spe_regs.bcr & CACHE_OPEN_MASK) >> 1
#define IS_PGO_ON(cpu) \
    ((cpu)->spe_regs.bcr & BCR_PGO_MASK) >> 2

#define BASE_MMIO_ADDR 0xff000000
#define BASE_MMU_ADDR 0xfffffd00
#define BASE_BIOS_ADDR 0xfffffe00
//BIOS 512 B

#define CPU_REG_PC 15
#define CPU_REG_SPE_BCR 0b10000
#define CPU_REG_SPE_SCR 0b10001
#define CPU_REG_SPE_FLAGS 0b10010
#define CPU_REG_SPE_PGBASE 0b10011
#define CPU_REG_ECALL_ECBASE 0b10100
//异常基址寄存器
#define CPU_REG_ECALL_ICBASE 0b10101
//中断基址寄存器
#define CPU_REG_ECALL_REASON 0b10110
//异常发生原因寄存器
#define CPU_REG_IRQ_REASON 0b10111
//中断发生原因寄存器
#define CPU_REG_RSP 14

#define ICODE_NOP 0b000000
#define ICODE_MOV 0b000001
#define ICODE_ALU 0b000010
#define ICODE_MLMR 0b000011

#define ICODE_PUSH 0b000100
//R类
#define ICODE_POP 0b000101
//R类

#define ICODE_ZWC 0b000110
//R类
#define ICODE_JMP 0b100000
#define ICODE_OCALL 0b100011

#define ICODE_ECALL 0b100010
//I类
#define ICODE_RET 0b100100
//R类
#define ICODE_RIRE 0b100101
//R类
#define ICODE_RERE 0b100110
//R类
//lier: Load IRQ EXCEPTION Reason %r1

//SCR控制器
#define ICODE_RSCR 0b110000
//R类
#define ICODE_LBCR 0b110001
//分页模式基地址
//R类

#define ICODE_LPGR 0b110010
//异常/中断进入的特权级寄存器
//R类
#define ICODE_LEPV 0b110011
//R类
#define ICODE_LIPV 0b110100
//异常/中断模式基址
//R类
#define ICODE_LIBR 0b110101
//R类
#define ICODE_LEBR 0b110110
//R类

#define ICODE_RET_EXFUNC 0x01
#define ICODE_IRET_EXFUNC 0x10
#define ICODE_ERET_EXFUNC 0x11

#define OPER_FLAG_1B 0b01
#define OPER_FLAG_2B 0b10
#define OPER_FLAG_4B 0b00
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

#define GET_MLMR_EXFUNC_ALU(exfunc) ((exfunc) >> 4) & 1
#define ALU_EXFUNC_MLMR_MASK 0x1
#define ALU_EXFUNC_ML 0x0
#define ALU_EXFUNC_MR 0x1

#define EXCOND_R1_ON(excond) ((excond) >> 3)
#define EXFLAG_R1_ON(exfunc) ((exfunc) >> 3)
#define IS_ITYPE(val) ((val) ^ 1)
#define IS_RTYPE(val) (val)
#define IS_PRIVILEGE_INSTRUCTION(icode) ((icode) >>4) == 0b11 

#define IS_ICODE_INVALID(icode,type) \
    (((icode) != ICODE_ALU \
        && (icode) != ICODE_ECALL \
        && (icode) != ICODE_JMP \
        && (icode) != ICODE_LEBR \
        && (icode) != ICODE_LIBR \
        && (icode) != ICODE_LPGR \
        && (icode) != ICODE_LBCR \
        && (icode) != ICODE_MLMR \
        && (icode) != ICODE_MOV \
        && (icode) != ICODE_OCALL \
        && (icode) != ICODE_RET \
        && (icode) != ICODE_RSCR \
        && (icode) != ICODE_RIRE \
        && (icode) != ICODE_RERE \
        && (icode) != ICODE_LEPV \
        && (icode) != ICODE_LIPV \
        && (icode) != ICODE_ZWC \
        && (icode) != ICODE_PUSH \
        && (icode) != ICODE_POP  \
        && (icode) != ICODE_NOP) \
    || ((( \
        (icode) == ICODE_PUSH || \
        (icode) == ICODE_POP || \
        (icode) == ICODE_ZWC || \
        (icode) == ICODE_RET || \
        (icode) == ICODE_RIRE || \
        (icode) == ICODE_RERE || \
        (icode) == ICODE_NOP || \
        IS_PRIVILEGE_INSTRUCTION(icode) \
        ) && !IS_RTYPE(type)) \
    ) || ( \
        ((icode) == ICODE_ECALL) && \
        !IS_ITYPE(type) \
    ))

#define ICODE_EXFLAG_MOV_MEM1(exflag) ((exflag) >> 1) & 1
#define ICODE_EXFLAG_MOV_MEM2(exflag) ((exflag) & 1)

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
    uint8_t irtype:1;
    uint8_t icode;
    uint8_t destReg:5;
    uint8_t destReg2:5;
    uint32_t valC_Extended;//用于乘除法
    uint32_t valC;
    uint32_t valB;
    uint8_t ExCond:3;
    uint8_t ExFunc:5;
    uint8_t ExFlag:5;
    uint8_t noexception:1;//指令应该继续执行吗?
} WCPUMemResult;

typedef struct {
    uint8_t irtype:1;
    uint32_t valC;
    uint32_t valC_Extended;//用于乘除法
    uint32_t valB;
    uint32_t valA;
    uint8_t icode;
    uint8_t destReg;
    uint8_t destReg2:5;
    uint8_t ExCond:3;
    uint8_t ExFunc:5;
    uint8_t ExFlag:5;
    uint8_t noexception:1;//指令应该继续执行吗?
}WCPUExecuteResult;

typedef struct {
    uint8_t irtype:1;
    uint8_t icode;
    uint8_t destReg;
    uint8_t destReg2:5;
    uint32_t valA;
    uint32_t valB;
    uint32_t valC;
    uint8_t ExCond:4;
    uint8_t ExFunc:5;
    uint8_t ExFlag:5;
    uint8_t noexception:1;//指令应该继续执行吗?
} WCPUDecodedData;

typedef struct {
    uint8_t irtype:1;
    uint8_t icode;
    uint32_t valC;
    uint8_t noexception:1;
    uint8_t ExFunc;
} WCPUWBResult;

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
    WCPUWBResult wb_result_reg;

    pthread_mutex_t clock_execution; //CPU 时序锁
    //真实电路都是按照时序来的，不需要这玩意，这个只是模拟时序
} WOLF_CPU;

void access_check(WOLF_CPU* cpu);
void execute(WOLF_CPU* cpu);
void fetch_data(WOLF_CPU* cpu);
void memory(WOLF_CPU* cpu);
void decode(WOLF_CPU* cpu);
void writeback(WOLF_CPU* cpu);
void update_PC(WOLF_CPU* cpu);
void ecall_proc(WOLF_CPU* cpu);

uint32_t getRegVal(WOLF_CPU* cpu,uint8_t lreg);
void write_reg_val(WOLF_CPU* cpu,uint8_t regnum,uint32_t value);

void start_cpu(WOLF_CPU* cpu);
WOLF_CPU* init_cpu();
void free_cpu(WOLF_CPU** cpu);
void init_env();
#endif