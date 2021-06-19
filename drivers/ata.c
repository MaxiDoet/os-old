#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/ata.h"
#include "../include/kernel/io.h"
#include "../include/kernel/kernel.h"
#include "../config.h"

char* ata_device_tree(bool primary, bool master);
void ata_device_debug(ata_dev_t dev, char* msg);
void ata_err_dump(ata_dev_t dev, uint8_t status);

uint8_t ata_init(ata_dev_t *dev, uint16_t port_base, bool master)
{
	dev->data_port = port_base;
	dev->error_port = port_base + 0x1;
	dev->sector_count_port = port_base + 0x2;
	dev->lba_low_port = port_base + 0x3;
	dev->lba_mid_port = port_base + 0x4;
	dev->lba_high_port = port_base + 0x5;
	dev->device_select_port = port_base + 0x6;
	dev->command_port = port_base + 0x7;
	dev->control_port = port_base + 0x206;
	dev->master = master;

	uint8_t status;

	outb(dev->device_select_port, master ? 0xA0 : 0xB0);
	outb(dev->control_port, 0);

	// Reset
	outb(dev->device_select_port, master ? 0xA0 : 0xB0);
	outb(dev->sector_count_port, 0);
	outb(dev->lba_low_port, 0);
	outb(dev->lba_mid_port, 0);
	outb(dev->lba_high_port, 0);

	// Identify
	outb(dev->command_port, 0xEC);
	status = inb(dev->command_port);
	if (status == 0) return 0;

	while (status & (1 << 7)) {
		status = inb(dev->command_port);
	}

	// Wait for data
	status = inb(dev->command_port);

	while ((!status & (1 << 3)) && (!status & (1 << 0))) {
		status = inb(dev->command_port);
	}

	if (status & (1 << 0)) return 0;
	ata_device_debug(*dev, "ready");

	// Read data
	uint16_t info[256];
	for (int i=0; i < 256; i++) {
		info[i] = inw(dev->data_port);
	}

	return 1;
}

char* ata_device_tree(bool primary, bool master)
{
	if (primary) {
		return (master) ? "ATA->Primary Master" : "ATA->Primary Slave";
	} else {
		return (master) ? "ATA->Secondary Master": "ATA->Secondary Slave";
	}
}

void ata_device_debug(ata_dev_t dev, char* msg)
{
	kdebug("[ata] %s: %s\r\n", ata_device_tree((dev.data_port == (ATA_PRIMARY_MASTER | ATA_PRIMARY_SLAVE)), dev.master), msg);
}

uint8_t ata_pio_wait_bsy(ata_dev_t dev)
{
	uint8_t status = inb(dev.command_port);

        while(status & (1 << 7)) {
		// Wait 400ns before reading the status register
		asm volatile ("nop");
            	asm volatile ("nop");
            	asm volatile ("nop");
            	asm volatile ("nop");
            	asm volatile ("nop");

		#ifdef DEBUG_ATA_BUSY
			ata_device_debug(dev, "busy");
		#endif

                status = inb(dev.command_port);

		if (status & (1 << 0)) ata_err_dump(dev, status);
        }

	return status;
}

void ata_err_dump(ata_dev_t dev, uint8_t status)
{
	char* error_text;
	uint8_t error;

	// Error bit is set
	if (status & (1 << 0)) {
		uint8_t error = inb(dev.error_port);

		if (error & (1 << 0)) {
			error_text = "Address Mark not found";
		} else if (error & (1 << 1)) {
			error_text = "Track zero not found";
		} else if (error & (1 << 2)) {
			error_text = "Aborted command";
		} else if (error & (1 << 3)) {
			error_text = "Media change request";
		} else if (error & (1 << 4)) {
			error_text = "ID not found";
		} else if (error & (1 << 5)) {
			error_text = "Media changed";
		} else if (error & (1 << 6)) {
			error_text = "Uncorrectable data error";
		} else if (error & (1 << 7)) {
			error_text = "Bad Block detected";
		} else {
			error_text = "Unknown error";
		}
	}

	ata_device_debug(dev, error_text);
}

uint8_t ata_pio_wait_drq(ata_dev_t dev)
{
	uint8_t status;

        while(!(status & (1 << 3))) {
		#ifdef DEBUG_ATA_WAIT_DRQ
			ata_device_debug(dev, "waiting drq");
		#endif

		if (status & (1 << 0)) ata_err_dump(dev, status);

                status = inb(dev.command_port);
        }

        return status;
}

void ata_pio_read(ata_dev_t dev, uint32_t lba, uint8_t sector_count, uint16_t *buf)
{
	if (!dev.data_port) return;

	// Select device
	outb(dev.device_select_port, (dev.master ? 0xE0 : 0xF0) | ((lba & 0x0F000000) >> 24));
	ata_pio_wait_bsy(dev);

	outb(dev.error_port, 0);
	outb(dev.sector_count_port, sector_count);
	outb(dev.lba_low_port, lba & 0x000000FF);
	outb(dev.lba_mid_port, (lba & 0x0000FF00) >> 8);
	outb(dev.lba_high_port, (lba & 0x00FF0000) >> 16);
	outb(dev.command_port, 0x20);

	for (int i=0; i < sector_count; i++) {
		ata_pio_wait_bsy(dev);
		ata_pio_wait_drq(dev);

		for (int j=0; j < 256; j++) {
			buf[j] = inw(dev.data_port);
		}

		buf+=256;
	}
}

void ata_pio_write(ata_dev_t dev, uint32_t sector, uint16_t* buf, uint32_t count)
{
	// Select sector
        outb(dev.device_select_port, (dev.master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
        outb(dev.error_port, 0);
        outb(dev.sector_count_port, 1);
        outb(dev.lba_low_port, sector & 0x000000FF);
        outb(dev.lba_mid_port, (sector & 0x0000FF00) >> 8);
        outb(dev.lba_high_port, (sector & 0x00FF0000) >> 16);
        outb(dev.command_port, 0x20);

	for(int i = 0; i < 256; i++) {
		outw(dev.data_port, buf[i]);
    	}
}

void ata_pio_flush(ata_dev_t dev)
{
	outb(dev.device_select_port, (dev.master ? 0xE0 : 0xF0));
	outb(dev.command_port, 0xE7);

	uint8_t status = ata_pio_wait_bsy(dev);

	if (status & 0x01) {
		return;
	}
}
