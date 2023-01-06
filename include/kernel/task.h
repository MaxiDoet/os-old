#ifndef _task_h
#define _task_h

typedef struct task_registers_t {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t eip;
    uint32_t eflags;
    //uint32_t cr3;
} task_registers_t;

typedef struct task_t {
    task_registers_t regs;
} task_t;

void tasking_init();
void tasking_schedule(cpu_state *state);

void task_create(void main());

#endif