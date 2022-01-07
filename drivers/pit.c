#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/pit.h"
#include "../include/kernel/io.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/kernel.h"

uint32_t pit_ticks;

void pit_set_divisor(uint16_t divisor)
{
	outb(PIT_CHANNEL_0_DATA_PORT, divisor & 0xFF);
        outb(PIT_CHANNEL_0_DATA_PORT, (divisor & 0xFF00) >> 8);
}

void pit_set_freq(uint16_t freq)
{
	uint16_t divisor = 1193182 / freq;
	pit_set_divisor(divisor);
}

void pit_irq_handler()
{
	pit_ticks++;
	pit_set_freq(10);
}

void pit_sleep(uint32_t ticks)
{
	uint32_t now = pit_ticks;

	while(pit_ticks < now + ticks) {
		asm("hlt");
	}
}

void pit_init()
{
	irq_install_handler(0, pit_irq_handler);

	outb(PIT_MODE_PORT, 0xD |  // Mode: Rate generator
			    0x30); // Access mode: lobyte/hibyte

	pit_set_freq(10);
	//pit_set_divisor(65535);
}

