#ifndef _task_h
#define _task_h

#include "../include/kernel/cpu/cpu.h"

typedef struct task_t {
    char *name;

    cpu_state_t *state;
    uint8_t stack[4096];
} task_t;

void scheduler_init(void (*kernel_task)());
cpu_state_t *scheduler_schedule(cpu_state_t *state);

void scheduler_task_create(uint32_t entry, char *name);

#endif