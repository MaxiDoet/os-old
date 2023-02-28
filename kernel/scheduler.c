#include <stdint.h>
#include <stdbool.h>

#include "../include/kernel/kernel.h"
#include "../include/kernel/scheduler.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/mem/heap.h"
#include "../include/drivers/pit.h"
#include "../libc/include/string.h"

bool scheduler_running = false;
task_t task_list[10];
uint8_t current_task;
uint8_t task_count;

void scheduler_task_create(uint32_t entry, char *name)
{
    task_t *task = &task_list[task_count++];

    task->name = (char *) malloc(strlen(name) + 1);
    memcpy(task->name, name, strlen(name) + 1);

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

void scheduler_init(void (*kernel_task)())
{
    current_task = 0;
    task_count = 0;

    /* Init kernel task */
    scheduler_task_create((uint32_t) kernel_task, "kernel\0");

    scheduler_running = true;

    (*kernel_task)();
}

cpu_state_t *scheduler_schedule(cpu_state_t *state)
{
    if (!scheduler_running) {
        return state;
    }

    cpu_cli();

    // Save current state
    task_list[current_task].state = state;

    kdebug("Current %s(%d) | eax: %x ebx: %x ecx: %x edx: %x esi: %x edi: %x ebp: %x | eip: %x esp: %x\r\n", task_list[current_task].name, current_task, state->eax, state->ebx, state->ecx, state->edx, state->esi, state->edi, state->ebp, state->eip, state->esp);

    if (++current_task >= task_count) {
        current_task %= task_count;
    }

    task_t *next_task = &task_list[current_task];

    kdebug("Next %s(%d) | eax: %x ebx: %x ecx: %x edx: %x esi: %x edi: %x ebp: %x | eip: %x esp: %x\r\n", task_list[current_task].name, current_task, next_task->state->eax, next_task->state->ebx, next_task->state->ecx, next_task->state->edx, next_task->state->esi, next_task->state->edi, next_task->state->ebp, next_task->state->eip, next_task->state->esp);

    cpu_sti();

    return next_task->state;
}