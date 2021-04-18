#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/ata.h"
#include "../include/kernel/asm.h"
#include "../include/kernel/kernel.h"

ata_dev_t ata_init(uint16_t port_base, bool master)
{
	ata_dev_t dev;

	dev.data_port = port_base;
	dev.error_port = port_base + 0x1;
	dev.sector_count_port = port_base + 0x2;
	dev.lba_low_port = port_base + 0x3;
	dev.lba_mid_port = port_base + 0x4;
	dev.lba_high_port = port_base + 0x5;
	dev.device_select_port = port_base + 0x6;
	dev.command_port = port_base + 0x7;
	dev.control_port = port_base + 0x206;
	dev.master = master;

	// Select drive
	outb(dev.device_select_port, master ? 0xA0 : 0xB0);
	outb(dev.control_port, 0);

	outb(dev.device_select_port, 0xA0);
	uint8_t status = inb(dev.command_port);
	if (status == 0xFF) {
		kdebug("err\r\n");
		return dev;
	}

	// Reset
	outb(dev.device_select_port, master ? 0xA0 : 0xB0);
	outb(dev.sector_count_port, 0);
	outb(dev.lba_low_port, 0);
	outb(dev.lba_mid_port, 0);
	outb(dev.lba_high_port, 0);

	// Identify
	outb(dev.command_port, 0xEC);
	status = inb(dev.command_port);
	if (status == 0x00) {
		kdebug("err\r\n");
	}

	// Waits until device is ready
	status = ata_busy_wait(dev);

	if (status & 0x01) {
		kdebug("err\r\n");
		return dev;
	}

	for(int i = 0; i < 256; i++) {
        	uint16_t data = inw(dev.data_port);
        	char *text = "  \0";
        	text[0] = (data >> 8) & 0xFF;
        	text[1] = data & 0xFF;

		kdebug(text);
    	}

	kdebug("\r\n");
}

uint8_t ata_busy_wait(ata_dev_t dev)
{
	uint8_t status;

	// Drive is busy
        while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) {
                status = inb(dev.command_port);
        }

	return status;
}

uint16_t ata_read(ata_dev_t dev, uint32_t sector, int count)
{
	// Select sector
	outb(dev.device_select_port, (dev.master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
	outb(dev.error_port, 0);
	outb(dev.sector_count_port, 1);
	outb(dev.lba_low_port, sector & 0x000000FF);
	outb(dev.lba_mid_port, (sector & 0x0000FF00) >> 8);
	outb(dev.lba_high_port, (sector & 0x00FF0000) >> 16);
	outb(dev.command_port, 0x20);

	uint8_t status = ata_busy_wait(dev);

	if (status & 0x01) {
		return 0;
	}

	for (int i=0; i < count; i +=2) {
		uint16_t data = inw(dev.data_port);

		char *text = "  \0";
		text[0] = data & 0xFF;

		if (i+1 < count) {
			text[1] = (data >> 8) & 0xFF;
		} else {
			text[1] = '\0';
		}

		return data;
	}

	for(int i = count + (count%2); i < 512; i += 2) {
		inb(dev.data_port);
	}
}

void ata_write(ata_dev_t dev, uint32_t sector, uint8_t* data, uint32_t count)
{
	// Select sector
        outb(dev.device_select_port, (dev.master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
        outb(dev.error_port, 0);
        outb(dev.sector_count_port, 1);
        outb(dev.lba_low_port, sector & 0x000000FF);
        outb(dev.lba_mid_port, (sector & 0x0000FF00) >> 8);
        outb(dev.lba_high_port, (sector & 0x00FF0000) >> 16);
        outb(dev.command_port, 0x20);

	for(int i = 0; i < count; i += 2) {
        	uint16_t data16 = data[i];
        	if(i+1 < count) {
            		data16 |= ((uint16_t)data[i+1]) << 8;
		}

		outw(dev.data_port, data16);

        	char *text = "  \0";
        	text[0] = (data16 >> 8) & 0xFF;
        	text[1] = data16 & 0xFF;
    	}

	for(int i = count + (count%2); i < 512; i += 2) {
		outw(dev.data_port, 0x0000);
	}
}

void ata_flush(ata_dev_t dev)
{
	outb(dev.device_select_port, (dev.master ? 0xE0 : 0xF0));
	outb(dev.command_port, 0xE7);

	uint8_t status = ata_busy_wait(dev);

	if (status & 0x01) {
		return;
	}
}
