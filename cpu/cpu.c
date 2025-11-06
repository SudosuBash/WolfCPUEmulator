#include "cpu.h"
#include <stdint.h>
#include <stdlib.h>
#include <cpu/cpu_mem.h>
uint64_t clk = 0;
void start_cpu(WOLF_CPU *cpu) {
    while (1) {
        fetchData(cpu);
        decode(cpu);
        execute(cpu);
        memory(cpu);
        clk += 1;
    }
}

void init_env() {
    WOLF_CPU* cpu = init_cpu();
    start_cpu(cpu);
}

WOLF_CPU* init_cpu() {
    WOLF_CPU* cpu = (WOLF_CPU*) malloc(sizeof(WOLF_CPU));
    if(cpu == NULL) goto FREE_CPU;
    WOLF_CPU_BUS_CONTROLLER* bus = init_bus();
    if(bus == NULL) goto FREE_BUS;
    WOLF_CPU_ECALL_CONTROLLER* ecall = init_ecall();
    if (ecall == NULL) goto FREE_ECALL;
    
    WOLF_CACHE_CONTROLLER* cache = init_cache_controller();
    if(cache == NULL) goto FREE_CACHE;

    WOLF_MEM_CONTROLLER* mem = init_mem_controller();
    if(mem == NULL) goto FREE_MEM;

    MACHINE_L1_CACHE_GROUP** l1_group = init_l1_group(CACHE_L1_GROUPS);
    if(l1_group == NULL) goto FREE_L1_GROUP;
    MACHINE_L2_CACHE_GROUP** l2_group = init_l2_group(CACHE_L2_GROUPS);
    if(l2_group == NULL) goto FREE_L2_GROUP;

    cpu->cache_controller = cache;
    cpu->bus = bus;
    cpu->ecall_controller = ecall;
    cpu->mem_controller = mem;
    cpu->cache1 = l1_group;
    cpu->cache2 = l2_group;

    return cpu;

FREE_L2_GROUP: free_l1_group(&l1_group,CACHE_L1_GROUPS);
FREE_L1_GROUP: free_mem(&mem);
FREE_MEM: free_cache(&cache);
FREE_CACHE: free_ecall(&ecall);
FREE_ECALL: free_bus(&bus);
FREE_BUS: free(cpu);
FREE_CPU: return NULL;
}

void free_cpu(WOLF_CPU** cpu) {
    WOLF_CPU* ocpu = *cpu;
    if(ocpu != NULL) {
        free_cache(ocpu->cache_controller);
        free_ecall(ocpu->ecall_controller);
        free_bus(ocpu->bus);
        free(*cpu);
        *cpu = NULL;
    }
}