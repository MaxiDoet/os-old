#include <stdint.h>
#include <stdbool.h>

#include "../include/kernel/kernel.h"
#include "../include/kernel/task.h"
#include "../include/kernel/irq.h"
#include "../include/drivers/pit.h"

task_t *task_list[10];
uint8_t current_task;
uint8_t task_count;

void tasking_init()
{
    current_task = 0;

    pit_init();
}

cpu_state_t *tasking_schedule(cpu_state_t *state)
{
    //kdebug("tasking_schedule() current_task: %d task_count: %d\r\n", current_task, task_count);

    if (task_count == 0) {
        return state;
    }

    task_list[current_task]->state = state;

    if (++current_task >= task_count) {
        current_task %= task_count;
    }

    return task_list[current_task]->state;
}

void task_create(void main())
{
    task_t task;

    task.state = (cpu_state_t *) (task.stack + 4096 - sizeof(cpu_state_t));
    
    task.state->eax = 0;
    task.state->ebx = 0;
    task.state->ecx = 0;
    task.state->edx = 0;

    task.state->esi = 0;
    task.state->edi = 0;
    task.state->ebp = 0;

    task.state->eip = (uint32_t) main;
    //task.state->esp = (uint32_t) task.stack;

    task.state->cs = 0x10;
    task.state->ss = 0x00;

    task.state->eflags = 0x202;

    task_list[task_count++] = &task;
}