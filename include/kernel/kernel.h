#ifndef _kernel_h
#define _kernel_h

#include <stdint.h>

void kmain(unsigned long magic, unsigned long addr);
void kpanic(char* msg);
void kdebug(char* format, ...);

#endif
