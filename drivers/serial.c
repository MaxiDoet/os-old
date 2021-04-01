#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/serial.h"

#include "../include/kernel/asm.h"

void serial_init()
{
	outb(SERIAL_DATA_PORT + 3, 0x80); // Allow access to baud rate registers
	outb(SERIAL_DATA_PORT + 0, 0x03); // Send low byte of divisor
	outb(SERIAL_DATA_PORT + 1, 0x00); // Send high byte of divisor
	outb(SERIAL_DATA_PORT + 3, 0x03); // 8bits, one stopbyte
	outb(SERIAL_DATA_PORT + 2, 0xC7);
	outb(SERIAL_DATA_PORT + 4, 0x0B); // Enable irqs
	outb(SERIAL_DATA_PORT + 4, 0x0F); // Normal mode

	// Todo: Test if chip works, set to loopback mode, send some data and see if we receive data
}

int serial_avaiable()
{
	return inb(SERIAL_DATA_PORT + 5) & 1;
}

uint8_t serial_read()
{
	// Wait until we receive some data
	while(serial_avaiable() == 0);

	return inb(SERIAL_DATA_PORT);
}

int serial_transmit_empty()
{
	return inb(SERIAL_DATA_PORT + 5) & 0x20;
}

void serial_write(char c)
{
	// Wait until all data was sent
	while(serial_transmit_empty() == 0);

	outb(SERIAL_DATA_PORT, c);
}
