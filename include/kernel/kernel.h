#ifndef _kernel_h
#define _kernel_h

#include <stdint.h>

typedef struct cpu_state {
    uint32_t   eax;
    uint32_t   ebx;
    uint32_t   ecx;
    uint32_t   edx;
    uint32_t   esi;
    uint32_t   edi;
    uint32_t   ebp;

    uint32_t   intr;

    uint32_t   eip;
    uint32_t   esp;
    uint32_t   cs;
    uint32_t   eflags;
    uint32_t   ss;
} cpu_state;

void kmain(unsigned long magic, unsigned long addr);
void kpanic(char* msg);
void kdebug(char* format, ...);

#endif
