#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/serial.h"

#include "../include/kernel/io.h"

#define REG_DATA	0x00
#define REG_IEN		0x01
#define REG_BLSB	0x00
#define REG_BMSB	0x01
#define REG_II		0x02
#define REG_LCTL	0x03
#define REG_MCTL	0x04
#define REG_LSTAT	0x05
#define REG_MSTAT	0x06

#define IEN_DATA			(1 << 0)
#define IEN_TXEMPTY			(1 << 1)
#define IEN_ERROR			(1 << 2)
#define IEN_STATUS_CHANGE	(1 << 3)

#define LCTL_DLAB	(1 << 7)

#define LSTAT_DR	(1 << 0)
#define LSTAT_THRE	(1 << 5)

void serial_init(uint16_t data_port)
{
	/* Disable interrupts */
	outb(data_port + REG_IEN, 0);
	
	/* Unlock baud rate regs and set baudrate to 115200 baud */
	outb(data_port + REG_LCTL, LCTL_DLAB);
	outb(data_port + REG_BLSB, 0x00);
	outb(data_port + REG_BMSB, 0x00);
	outb(data_port + REG_LCTL, 0x00);

	outb(data_port + REG_LCTL, 0x03); // 8bits, one stopbyte
	outb(data_port + REG_II, 0xC7);
	outb(data_port + 4, 0x0B); // Enable irqs
	outb(data_port + 4, 0x0F); // Normal mode
}

uint8_t serial_avaiable(uint16_t data_port)
{
	return inb(data_port + REG_LSTAT) & LSTAT_DR;
}

uint8_t serial_read(uint16_t data_port)
{
	// Wait until we receive some data
	while(serial_avaiable(data_port) == 0);

	return inb(data_port);
}

uint8_t serial_transmit_empty(uint16_t data_port)
{
	return inb(data_port + REG_LSTAT) & LSTAT_THRE;
}

void serial_write(uint16_t data_port, char c)
{
	// Wait until all data was sent
	while(serial_transmit_empty(data_port) == 0);

	outb(data_port, c);
}
