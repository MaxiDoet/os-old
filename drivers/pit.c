#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/pit.h"
#include "../include/kernel/io.h"

void beep(uint32_t freq)
{
	uint32_t divisor;
	uint8_t tmp;

	divisor = 1193180 / freq;
	outb(0x43, 0xB6);
	outb(0x42, (uint8_t) (divisor));
	outb(0x42, (uint8_t) (divisor >> 8));

	tmp = inb(0x61);
	if (tmp != (tmp | 3)) {
		outb(0x61, tmp | 3);
	}
}
