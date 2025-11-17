#include <cpu.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <debug/debug_io.h>
#include <mmio_devices/device_init.h>
#include <unistd.h>
uint64_t clk = 0;
void start_cpu(WOLF_CPU *cpu) {
    while (1) {
        pthread_mutex_lock(&cpu->clock_execution);
        fetch_data(cpu);
        access_check(cpu);
        decode(cpu);
        execute(cpu);
        ecall_proc(cpu);
        memory(cpu);
        writeback(cpu);
        update_PC(cpu);
        clk += 1;
        pthread_mutex_unlock(&cpu->clock_execution);
    }
}

void init_env() {
    WOLF_CPU* cpu = init_cpu();
    init_devices(cpu->bus);
    start_cpu(cpu);
    free_cpu(&cpu);
}

WOLF_CPU* init_cpu() {
    WOLF_CPU* cpu = (WOLF_CPU*) calloc(1,sizeof(WOLF_CPU));
    if(cpu == NULL) goto FREE_CPU;
    WOLF_CPU_BUS_CONTROLLER* bus = init_bus();
    if(bus == NULL) goto FREE_BUS;
    WOLF_CPU_ECALL_CONTROLLER* ecall = init_ecall();
    if (ecall == NULL) goto FREE_ECALL;
    
    WOLF_CACHE_CONTROLLER* cache = init_cache_controller();
    if(cache == NULL) goto FREE_CACHE;

    WOLF_MEM_CONTROLLER* mem = init_mem_controller();
    if(mem == NULL) goto FREE_MEM;
    WOLF_CPU_MMU_CONTROLLER* mmu = init_mmu_controller();
    if(mmu == NULL) goto FREE_MMU;
    MACHINE_L1_CACHE_GROUP** l1_group = init_l1_group(CACHE_L1_GROUPS);
    if(l1_group == NULL) goto FREE_L1_GROUP;
    MACHINE_L2_CACHE_GROUP** l2_group = init_l2_group(CACHE_L2_GROUPS);
    if(l2_group == NULL) goto FREE_L2_GROUP;
    WOLF_ALU* alu = init_alu();
    if(alu == NULL) goto FREE_ALU;
    cpu->alu = alu;
    cpu->cache_controller = cache;
    cpu->bus = bus;
    cpu->ecall_controller = ecall;
    cpu->mem_controller = mem;
    cpu->mmu = mmu;
    cpu->cache1 = l1_group;
    cpu->cache2 = l2_group;
    cpu->clock_execution = PTHREAD_MUTEX_INITIALIZER;
    cpu->pc = BASE_BIOS_ADDR; //刚开始初始化pc为BASE_BIOS_ADDR，转去执行BIOS的程序
    return cpu;

FREE_ALU: free_l2_group(&l2_group,CACHE_L2_GROUPS);
FREE_L2_GROUP: free_l1_group(&l1_group,CACHE_L1_GROUPS);
FREE_L1_GROUP: free_mmu_controller(&mmu);
FREE_MMU: free_mem(&mem);
FREE_MEM: free_cache(&cache);
FREE_CACHE: free_ecall(&ecall);
FREE_ECALL: free_bus(&bus);
FREE_BUS: free(cpu);
FREE_CPU: return NULL;
}

void free_cpu(WOLF_CPU** cpu) {
    WOLF_CPU* ocpu = *cpu;
    if(ocpu != NULL) {
        free_cache(&(ocpu->cache_controller));
        free_ecall(&(ocpu->ecall_controller));
        free_bus(&(ocpu->bus));
        free_alu(&(ocpu->alu));
        free(*cpu);
        *cpu = NULL;
    }
}

uint32_t getRegVal(WOLF_CPU* cpu,uint8_t lreg) {
    if(lreg < MAX_GEN_REGISTER_COUNT) {
        return cpu->gen_regs.r[lreg];
    }
    switch(lreg) {
        case CPU_REG_SPE_PGBASE:
            return cpu->spe_regs.pg_mode_base_addr_reg;
        case CPU_REG_ECALL_ECBASE:
            return cpu->ecall_regs.mpc;
        case CPU_REG_ECALL_ICBASE:
            return cpu->irq_regs.mpc;
        case CPU_REG_SPE_BCR:
            return cpu->spe_regs.bcr;
        case CPU_REG_SPE_FLAGS:
            return cpu->spe_regs.flags;
        case CPU_REG_PC:
            return cpu->pc;
    }
    
}


void write_reg_val(WOLF_CPU* cpu,uint8_t regnum,uint32_t value) {
    if(regnum < 15) {
        cpu->gen_regs.r[regnum] = value;
    }
    switch (regnum)
    {
    case CPU_REG_ECALL_ECBASE:
        cpu->ecall_regs.mep = value;
        break;
    case CPU_REG_ECALL_ICBASE:
        cpu->irq_regs.mep = value;
        break;
    case CPU_REG_SPE_BCR:
        cpu->spe_regs.bcr = value & 0xff;
        break;
    case CPU_REG_SPE_SCR:
        cpu->spe_regs.scr = value & 0xff;
        break;
    case CPU_REG_SPE_PGBASE:
        cpu->spe_regs.pg_mode_base_addr_reg = value;
        break;
    default:
        break;
    }
}