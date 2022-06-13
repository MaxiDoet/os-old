#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/ata.h"
#include "../include/kernel/io.h"
#include "../include/kernel/kernel.h"
#include "../config.h"

#define ATA_PRIMARY 	0x1F0
#define ATA_SECONDARY 	0x1E8

#define ATA_ERROR_AMNF 	(1 << 0)
#define ATA_ERROR_TKZNF (1 << 1)
#define ATA_ERROR_ABRT	(1 << 2)
#define ATA_ERROR_MCR	(1 << 3)
#define ATA_ERROR_IDNF	(1 << 4)
#define ATA_ERROR_MC	(1 << 5)
#define ATA_ERROR_UNC	(1 << 6)
#define ATA_ERROR_BBK	(1 << 7)

#define ATA_STATUS_ERR	(1 << 0)
#define ATA_STATUS_IDX	(1 << 1)
#define ATA_STATUS_CORR	(1 << 2)
#define ATA_STATUS_DRQ	(1 << 3)
#define ATA_STATUS_SRV	(1 << 4)
#define ATA_STATUS_DF	(1 << 5)
#define ATA_STATUS_RDY	(1 << 6)
#define ATA_STATUS_BSY	(1 << 7)

#define ATA_COMMAND_READ_SECTORS 	0x20
#define ATA_COMMAND_IDENTIFY 		0xEC

#define ATA_CONTROL_SRST			(0 << 2)

#define ATA_REGISTER_DATA 				0x00
#define ATA_REGISTER_ERROR				0x01
#define ATA_REGISTER_FEATURES			0x01
#define ATA_REGISTER_SECTOR_COUNT		0x02
#define ATA_REGISTER_LBA_LOW			0x03
#define ATA_REGISTER_LBA_MID			0x04
#define ATA_REGISTER_LBA_HIGH			0x05
#define ATA_REGISTER_SELECT				0x06
#define ATA_REGISTER_STATUS				0x07
#define ATA_REGISTER_COMMAND			0x07
#define ATA_REGISTER_PRIMARY_CONTROL	0x3F6
#define ATA_REGISTER_SECONDARY_CONTROL	0x376

void ata_device_debug(ata_dev_t *dev, char* msg)
{
	#ifdef DEBUG_ATA
	char* prefix;

	if (dev->primary && dev->master) {
		prefix = "ATA->Primary Master";
	} else if (dev->primary && !dev->master) {
		prefix = "ATA->Primary Slave";
	} else if (!dev->primary && dev->master) {
		prefix = "ATA->Secondary Master";
	} else {
		prefix = "ATA->Secondary Slave";
	}

	kdebug("[ata] %s | %s\r\n", prefix, msg);
	#endif
}

void ata_err_dump(ata_dev_t *dev, uint8_t status)
{
	char* error_text;
	uint8_t error;

	if (status & (1 << 0)) {
		uint8_t error = inb(dev->io_base + ATA_REGISTER_ERROR);

		if (error & ATA_ERROR_AMNF) {
			error_text = "Address Mark not found";
		} else if (error & ATA_ERROR_TKZNF) {
			error_text = "Track zero not found";
		} else if (error & ATA_ERROR_ABRT) {
			error_text = "Aborted command";
		} else if (error & ATA_ERROR_MCR) {
			error_text = "Media change request";
		} else if (error & ATA_ERROR_IDNF) {
			error_text = "ID not found";
		} else if (error & ATA_ERROR_MC) {
			error_text = "Media changed";
		} else if (error & ATA_ERROR_UNC) {
			error_text = "Uncorrectable data error";
		} else if (error & ATA_ERROR_BBK) {
			error_text = "Bad Block detected";
		} else {
			error_text = "Unknown error";
		}
	}

	ata_device_debug(dev, error_text);
}

uint8_t ata_pio_wait_bsy(ata_dev_t *dev)
{
	uint8_t status = inb(dev->io_base + ATA_REGISTER_STATUS);

    while(status & ATA_STATUS_BSY) {
		asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");

		ata_device_debug(dev, "waiting busy");

	    status = inb(dev->io_base + ATA_REGISTER_STATUS);

		if (status & ATA_STATUS_ERR) {
			ata_err_dump(dev, status);
		}
    }

	return status;
}

uint8_t ata_pio_wait_drq(ata_dev_t *dev)
{
	uint8_t status = inb(dev->io_base + ATA_REGISTER_STATUS);

    while(!(status & ATA_STATUS_DRQ)) {
		ata_device_debug(dev, "waiting drq");

		if (status & ATA_STATUS_ERR) {
			ata_err_dump(dev, status);
		}

		status = inb(dev->io_base + ATA_REGISTER_STATUS);
	}
	
	return status;
}

uint8_t ata_pio_wait_rdy(ata_dev_t *dev)
{
	uint8_t status = inb(dev->io_base + ATA_REGISTER_STATUS);

    while(!(status & ATA_STATUS_RDY)) {
		ata_device_debug(dev, "waiting rdy");

		if (status & ATA_STATUS_ERR) {
			ata_err_dump(dev, status);
		}

		status = inb(dev->io_base + ATA_REGISTER_STATUS);
	}
	
	return status;
}

void ata_pio_reset(ata_dev_t *dev)
{
	outb(dev->master ? ATA_REGISTER_PRIMARY_CONTROL : ATA_REGISTER_SECONDARY_CONTROL, ATA_CONTROL_SRST);
	outb(dev->master ? ATA_REGISTER_PRIMARY_CONTROL : ATA_REGISTER_SECONDARY_CONTROL, 0);

	ata_pio_wait_bsy(dev);
	ata_pio_wait_rdy(dev);
}

void ata_pio_read(ata_dev_t *dev, uint32_t lba, uint8_t sector_count, uint16_t *buf)
{
	if (!dev->ready) return;

	// Enable LBA mode and send upper LBA bits
	outb(dev->io_base + ATA_REGISTER_SELECT, (dev->master ? 0xE0 : 0xF0) | ((lba & 0x0F000000) >> 24));
	ata_pio_wait_bsy(dev);

	/* Set sector count and write LBA */
	outb(dev->io_base + ATA_REGISTER_ERROR, 0);
	outb(dev->io_base + ATA_REGISTER_SECTOR_COUNT, sector_count);
	outb(dev->io_base + ATA_REGISTER_LBA_LOW, lba & 0x000000FF);
	outb(dev->io_base + ATA_REGISTER_LBA_MID, (lba & 0x0000FF00) >> 8);
	outb(dev->io_base + ATA_REGISTER_LBA_HIGH, (lba & 0x00FF0000) >> 16);
	outb(dev->io_base + ATA_REGISTER_COMMAND, ATA_COMMAND_READ_SECTORS);

	for (int i=0; i < sector_count; i++) {
		ata_pio_wait_bsy(dev);
		ata_pio_wait_drq(dev);

		for (int j=0; j < 256; j++) {
			buf[j] = inw(dev->io_base + ATA_REGISTER_DATA);
		}

		buf+=256;
	}
}

uint8_t ata_init(ata_dev_t *dev, bool primary, bool master)
{
	dev->primary = primary;
	dev->master = master;

	dev->io_base = primary ? ATA_PRIMARY : ATA_SECONDARY;

	dev->ready = false;

	uint8_t status;
	uint32_t timeout;

	outb(dev->master ? ATA_REGISTER_PRIMARY_CONTROL : ATA_REGISTER_SECONDARY_CONTROL, 0);

	// Select drive
	outb(dev->io_base + ATA_REGISTER_SELECT, master ? 0xA0 : 0xB0);

	// Reset
	outb(dev->io_base + ATA_REGISTER_SELECT, master ? 0xA0 : 0xB0);
	outb(dev->io_base + ATA_REGISTER_SECTOR_COUNT, 0);
	outb(dev->io_base + ATA_REGISTER_LBA_LOW, 0);
	outb(dev->io_base + ATA_REGISTER_LBA_MID, 0);
	outb(dev->io_base + ATA_REGISTER_LBA_HIGH, 0);

	// Identify
	outb(dev->io_base + ATA_REGISTER_COMMAND, ATA_COMMAND_IDENTIFY);
	status = inb(dev->io_base + ATA_REGISTER_STATUS);

	timeout = 1000;
	while (status & ATA_STATUS_BSY) {
		status = inb(dev->io_base + ATA_REGISTER_STATUS);

		timeout--;
		if (timeout < 1) return 0;
	}

	// Wait until all data is transfered
	status = inb(dev->io_base + ATA_REGISTER_STATUS);

	timeout = 1000;
	while ((!status & ATA_STATUS_DRQ) && (!status & ATA_STATUS_ERR)) {
		status = inb(dev->io_base + ATA_REGISTER_STATUS);

		timeout--;
		if (timeout < 1) return 0;
	}

	if (status & ATA_STATUS_ERR) {
		ata_err_dump(dev, status);
		return 0;
	}

	ata_device_debug(dev, "ready");

	// Read data
	uint16_t info[256];
	for (int i=0; i < 256; i++) {
		info[i] = inw(dev->io_base + ATA_REGISTER_DATA);
	}

	dev->ready = true;
	return 1;
}

/*
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
*/