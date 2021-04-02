#ifndef _kernel_h
#define _kernel_h

void kmain(unsigned long magic, unsigned long addr);
void kpanic(int exception);
void kdebug(int data_port, const char* format, ...);

#endif
