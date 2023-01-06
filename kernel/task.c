#include <stdint.h>
#include <stdbool.h>

#include "../include/kernel/kernel.h"
#include "../include/kernel/task.h"
#include "../include/kernel/irq.h"
#include "../include/drivers/pit.h"

task_t task_list[10];
uint8_t current_task;
uint8_t task_count;

void tasking_init()
{
    current_task = 0;

    pit_init();
}

void tasking_schedule(cpu_state *state)
{

}

void task_create(void main())
{
    task_list[task_count++].regs.eip = (uint32_t) main;
}