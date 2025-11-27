#ifndef _WOLF_DEBUG_IO
#define _WOLF_DEBUG_IO

#include <global.h>
#include <stdio.h>
#include <cpu.h>

#define DEBUG_CMD_STATUS_CONTINUE 0xf0
typedef struct HASHTABLE_ELEMENT{
    uint32_t break_pc;
    struct HASHTABLE_ELEMENT* next;
} HASHTABLE_ELEMENT;

typedef struct BREAKPOINT_CONTROLLER BREAKPOINT_CONTROLLER;
typedef void (*set_breakpoint_func)(BREAKPOINT_CONTROLLER* controller,uint32_t pc);
typedef void (*del_breakpoint_func)(BREAKPOINT_CONTROLLER* controller,uint32_t pc);
typedef HASHTABLE_ELEMENT* (*get_breakpoint_func)(BREAKPOINT_CONTROLLER* controller,WOLF_CPU* cpu,uint32_t pc);
typedef void (*visit_memory_func)(BREAKPOINT_CONTROLLER* controller,WOLF_CPU* cpu,uint32_t pc);

struct BREAKPOINT_CONTROLLER{
    set_breakpoint_func set_breakpoint;
    del_breakpoint_func del_breakpoint;
    get_breakpoint_func get_breakpoint;
    visit_memory_func visit_memory;
    HASHTABLE_ELEMENT* hashtable[32768];
};

uint32_t break_execution(WOLF_CPU* cpu);

void init_break_execution();
void free_break_execution();
#endif