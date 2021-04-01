#ifndef _asm_h
#define _asm_h
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void outb(uint16_t port, uint8_t data);
uint8_t inb(uint16_t port);
void outl(uint16_t port, uint32_t data);
uint32_t inl(uint16_t port);

#endif
