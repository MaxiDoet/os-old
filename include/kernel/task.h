#ifndef _task_h
#define _task_h

typedef struct task_t {
    cpu_state_t *state;
    uint8_t stack[4096];
} task_t;

void tasking_init();
cpu_state_t *tasking_schedule(cpu_state_t *state);

void task_create(void main());

#endif