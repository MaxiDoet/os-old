#include <stdint.h>
#include <stdbool.h>

#include "../include/kernel/kernel.h"
#include "../include/kernel/scheduler.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/mem/heap.h"
#include "../include/drivers/pit.h"
#include "../libc/include/string.h"

bool multitasking_active = false;
task_t task_list[10];
int current_task;
int task_count;

void scheduler_init()
{
    current_task = 0;
    task_count = 0;
    multitasking_active = true;
}

cpu_state_t *scheduler_schedule(cpu_state_t *state)
{
    if (!multitasking_active || task_count == 0) {
        return state;
    }

    // Save current state
    task_list[current_task].state = state;

    kdebug("Current %d | eax: %x ebx: %x ecx: %x edx: %x esi: %x edi: %x ebp: %x | eip: %x esp: %x\r\n", current_task, state->eax, state->ebx, state->ecx, state->edx, state->esi, state->edi, state->ebp, state->eip, state->esp);

    if (++current_task >= task_count) {
        current_task %= task_count;
    }

    task_t *next_task = &task_list[current_task];

    //memcpy(next_task->state, state, sizeof(cpu_state_t));

    kdebug("Next %d | eax: %x ebx: %x ecx: %x edx: %x esi: %x edi: %x ebp: %x | eip: %x esp: %x\r\n", current_task, next_task->state->eax, next_task->state->ebx, next_task->state->ecx, next_task->state->edx, next_task->state->esi, next_task->state->edi, next_task->state->ebp, next_task->state->eip, next_task->state->esp);

    return next_task->state;
}

void scheduler_task_create(uint32_t entry, char *name)
{
    task_t *task = &task_list[task_count++];

    task->name = (char *) malloc(strlen(name));
    memcpy(task->name, name, strlen(name));

    task->state = (cpu_state_t *) (task->stack + 4096 - sizeof(cpu_state_t));

    task->state->eax = 0;
    task->state->ebx = 0;
    task->state->ecx = 0;
    task->state->edx = 0;

    task->state->esi = 0;
    task->state->edi = 0;
    task->state->ebp = 0;

    task->state->eip = entry;
    task->state->esp = (uint32_t) task->stack;

    task->state->cs = 0x00;
    //task->state->ss = 0x00;

    task->state->eflags = 0x202;
}