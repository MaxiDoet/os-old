#include "../include/kernel/cpu/cpu.h"

void cpu_sti()
{
    asm volatile("sti");
}

void cpu_cli()
{
    asm volatile("cli");
}