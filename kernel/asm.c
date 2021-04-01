#include "../include/kernel/asm.h"

void outb(uint16_t port, uint8_t data)
{
        __asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

uint8_t inb(uint16_t port)
{
        uint8_t result;
        __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
        return result;
}

void outl(uint16_t port, uint32_t data)
{
        __asm__ volatile("outl %0, %1" : : "a" (data), "Nd" (port));
}

uint32_t inl(uint16_t port)
{
        uint32_t result;
        __asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (port));
        return result;
}

