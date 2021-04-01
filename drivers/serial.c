#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/serial.h"

#include "../include/kernel/asm.h"

void serial_init(int data_port)
{
	outb(data_port + 3, 0x80); // Allow access to baud rate registers
	outb(data_port + 0, 0x03); // Send low byte of divisor
	outb(data_port + 1, 0x00); // Send high byte of divisor
	outb(data_port + 3, 0x03); // 8bits, one stopbyte
	outb(data_port + 2, 0xC7);
	outb(data_port + 4, 0x0B); // Enable irqs
	outb(data_port + 4, 0x0F); // Normal mode

	// Todo: Test if chip works, set to loopback mode, send some data and see if we receive data
}

int serial_avaiable(int data_port)
{
	return inb(data_port + 5) & 1;
}

uint8_t serial_read(int data_port)
{
	// Wait until we receive some data
	while(serial_avaiable(data_port) == 0);

	return inb(data_port);
}

int serial_transmit_empty(int data_port)
{
	return inb(data_port + 5) & 0x20;
}

void serial_write(int data_port, char c)
{
	// Wait until all data was sent
	while(serial_transmit_empty(data_port) == 0);

	outb(data_port, c);
}
