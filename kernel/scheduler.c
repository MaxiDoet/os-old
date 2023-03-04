#include <stdint.h>
#include <stdbool.h>

#include "../include/kernel/kernel.h"
#include "../include/kernel/scheduler.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/mem/heap.h"
#include "../include/drivers/pit.h"
#include "../libc/include/string.h"

#define TASK_MAGIC 0x484848;

bool scheduler_running = false;
task_t *task_list;
uint8_t current_task;
uint8_t task_count;

void scheduler_task_create(uint32_t entry, char *name)
{
    task_t *task = &task_list[task_count++];

    task->name = (char *) malloc(strlen(name) + 1);
    memcpy(task->name, name, strlen(name) + 1);

    //task->state = (cpu_state_t *) (task->stack + 4096 - sizeof(cpu_state_t));

    task->state.eax = 0;
    task->state.ebx = 0;
    task->state.ecx = 0;
    task->state.edx = 0;

    task->state.esi = 0;
    task->state.edi = 0;
    task->state.ebp = 0;

    task->state.eip = entry;
    task->state.cs = 0x10;
    task->state.eflags = 0x202;
}

void task2()
{
    while(true) {
        kdebug("Hey 2\r\n");
    }
}

void scheduler_init(void (*kernel_task)())
{
    task_list = (task_t *) malloc(sizeof(task_t) * 1024);
    memset(task_list, 0x00, sizeof(task_t) * 1024);
    current_task = 0;
    task_count = 0;

    /* Init kernel task */
    scheduler_task_create((uint32_t) kernel_task, "kernel\0");

    scheduler_running = true;

    scheduler_task_create((uint32_t) task2, "kernel2\0");

    (*kernel_task)();
}

cpu_state_t *scheduler_schedule(cpu_state_t *state)
{
    if (!scheduler_running) {
        return state;
    }

    cpu_cli();

    // Save current state
    memcpy(&task_list[current_task].state, state, sizeof(cpu_state_t));

    for (uint32_t i=0; i < sizeof(cpu_state_t) / 4; i++) {
        kdebug("%x ", *((uint32_t *) state + i));
    }
    kdebug("\r\n");

    kdebug("Current %s(%d) | eax: %x ebx: %x ecx: %x edx: %x esi: %x edi: %x ebp: %x intr: %x error: %x eip: %x cs: %x eflags: %x esp: %x ss: %x\r\n", task_list[current_task].name, current_task, state->eax, state->ebx, state->ecx, state->edx, state->esi, state->edi, state->ebp, state->intr, state->error, state->eip, state->cs, state->eflags, state->esp, state->ss);

    if (++current_task >= task_count) {
        current_task %= task_count;
    }

    task_t *next_task = &task_list[current_task];

    kdebug("Next %s(%d) | eax: %x ebx: %x ecx: %x edx: %x esi: %x edi: %x ebp: %x intr: %x error: %x eip: %x cs: %x eflags: %x esp: %x ss: %x\r\n", task_list[current_task].name, current_task, next_task->state.eax, next_task->state.ebx, next_task->state.ecx, next_task->state.edx, next_task->state.esi, next_task->state.edi, next_task->state.ebp, next_task->state.intr, next_task->state.error, next_task->state.eip, next_task->state.cs, next_task->state.eflags, next_task->state.esp, next_task->state.ss);

    memcpy(state, &next_task->state, sizeof(cpu_state_t));

    cpu_sti();

    return state;
}