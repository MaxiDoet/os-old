#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../libc/include/string.h"
#include "../include/drivers/ata.h"
#include "../include/kernel/io/io.h"
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

#define ATA_COMMAND_READ_SECTORS 		0x20
#define ATA_COMMAND_WRITE_SECTORS		0x30
#define ATA_COMMAND_CACHE_FLUSH			0xE7
#define ATA_COMMAND_IDENTIFY 			0xEC

#define ATA_IDENTIFY_SERIAL				0x14
#define ATA_IDENTIFY_MODEL 				0x36

#define ATA_CONTROL_SRST				(0 << 2)

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

void ata_device_print(ata_dev_t *dev, char* msg)
{
	kdebug("%s bus | %s | %s\r\n", dev->primary ? "Primary" : "Secondary", dev->master ? "Master" : "Slave", msg);
}

void ata_err_dump(ata_dev_t *dev, uint8_t status)
{
	char* error_text;
	uint8_t error;

	if (status & (1 << 0)) {
		uint8_t error = inb(dev->io_base + ATA_REGISTER_ERROR);

		kdebug("error: %x\r\n", error);

		switch(error) {
			case ATA_ERROR_AMNF:
				error_text = "Address Mark not found";
				break;

			case ATA_ERROR_TKZNF:
				error_text = "Track zero not found";
				break;

			case ATA_ERROR_ABRT:
				error_text = "Aborted command";
				break;

			case ATA_ERROR_MCR:
				error_text = "Media change request";
				break;

			case ATA_ERROR_IDNF:
				error_text = "ID not found";
				break;

			case ATA_ERROR_MC:
				error_text = "Media changed";
				break;

			case ATA_ERROR_UNC:
				error_text = "Uncorrectable data error";
				break;

			case ATA_ERROR_BBK:
				error_text = "Bad Block detected";
				break;

			default:
				error_text = "Unknown error";
				break;
		}
	}

	ata_device_print(dev, error_text);
}

uint8_t ata_pio_wait_err(ata_dev_t *dev)
{	
	for (uint8_t i=0; i < 4; i++) {
		inb(dev->io_base + ATA_REGISTER_STATUS);
	}

	uint8_t status = inb(dev->io_base + ATA_REGISTER_STATUS);

	if (status & ATA_STATUS_ERR) {
		ata_err_dump(dev, status);
		return ATA_RETURN_ERROR;
	}

	return ATA_RETURN_SUCCESS;
}

uint8_t ata_pio_wait_bsy(ata_dev_t *dev)
{
	uint8_t status = inb(dev->io_base + ATA_REGISTER_STATUS);

    while((status & ATA_STATUS_BSY)) {
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");

		/* This produces the error messages */
		if (ata_pio_wait_err(dev) != ATA_RETURN_SUCCESS) {
			return ATA_RETURN_ERROR;
		}

		status = inb(dev->io_base + ATA_REGISTER_STATUS);
    }

	return ATA_RETURN_SUCCESS;
}

uint8_t ata_pio_wait_drq(ata_dev_t *dev)
{	
	uint8_t status = inb(dev->io_base + ATA_REGISTER_STATUS);

	while (!(status & ATA_STATUS_DRQ)) {
		if (ata_pio_wait_err(dev) != ATA_RETURN_SUCCESS) {
			return ATA_RETURN_ERROR;
		}

		status = inb(dev->io_base + ATA_REGISTER_STATUS);
	}
	
	return ATA_RETURN_SUCCESS;
}

uint8_t ata_pio_wait_rdy(ata_dev_t *dev)
{
	uint8_t status = inb(dev->io_base + ATA_REGISTER_STATUS);

    while(!(status & ATA_STATUS_RDY)) {
		status = inb(dev->io_base + ATA_REGISTER_STATUS);
	}
	
	return ATA_RETURN_SUCCESS;
}

void ata_pio_reset(ata_dev_t *dev)
{
	outb(dev->master ? ATA_REGISTER_PRIMARY_CONTROL : ATA_REGISTER_SECONDARY_CONTROL, ATA_CONTROL_SRST);
	outb(dev->master ? ATA_REGISTER_PRIMARY_CONTROL : ATA_REGISTER_SECONDARY_CONTROL, 0);

	ata_pio_wait_bsy(dev);
	ata_pio_wait_rdy(dev);
}

uint8_t ata_pio_read(ata_dev_t *dev, uint32_t lba, uint8_t sector_count, uint16_t *buf)
{
	if (!dev->ready) return ATA_RETURN_NOT_READY;
	
	if (ata_pio_wait_bsy(dev) != ATA_RETURN_SUCCESS) {
		return ATA_RETURN_ERROR;
	}

	// Enable LBA mode and send upper LBA bits
	outb(dev->io_base + ATA_REGISTER_SELECT, (dev->master ? 0xE0 : 0xF0) | ((lba & 0x0F000000) >> 24));
	outb(dev->io_base + ATA_REGISTER_ERROR, 0);
	outb(dev->io_base + ATA_REGISTER_SECTOR_COUNT, sector_count);
	outb(dev->io_base + ATA_REGISTER_LBA_LOW, lba & 0x000000FF);
	outb(dev->io_base + ATA_REGISTER_LBA_MID, (lba & 0x0000FF00) >> 8);
	outb(dev->io_base + ATA_REGISTER_LBA_HIGH, (lba & 0x00FF0000) >> 16);
	outb(dev->io_base + ATA_REGISTER_COMMAND, ATA_COMMAND_READ_SECTORS);

	for (uint8_t i=0; i < sector_count; i++) {
		if (ata_pio_wait_drq(dev) != ATA_RETURN_SUCCESS) {
			return ATA_RETURN_ERROR;
		}

		for (int j=0; j < 256; j++) {
			buf[j] = inw(dev->io_base + ATA_REGISTER_DATA);
		}

		buf+=256;
	}
}

uint8_t ata_pio_write(ata_dev_t *dev, uint32_t lba, uint16_t* buf, uint8_t sector_count)
{
	if (!dev->ready) return ATA_RETURN_NOT_READY;

	// Enable LBA mode and send upper LBA bits
	outb(dev->io_base + ATA_REGISTER_SELECT, (dev->master ? 0xE0 : 0xF0) | ((lba & 0x0F000000) >> 24));
	
	/* Set sector count and write LBA */
	outb(dev->io_base + ATA_REGISTER_ERROR, 0);
	outb(dev->io_base + ATA_REGISTER_SECTOR_COUNT, sector_count);
	outb(dev->io_base + ATA_REGISTER_LBA_LOW, lba & 0x000000FF);
	outb(dev->io_base + ATA_REGISTER_LBA_MID, (lba & 0x0000FF00) >> 8);
	outb(dev->io_base + ATA_REGISTER_LBA_HIGH, (lba & 0x00FF0000) >> 16);
	outb(dev->io_base + ATA_REGISTER_COMMAND, ATA_COMMAND_WRITE_SECTORS);

	for (int i=0; i < sector_count; i++) {
		//ata_pio_wait_bsy(dev);
		//ata_pio_wait_drq(dev);

		for (int j=0; j < 256; j++) {
			outw(dev->io_base + ATA_REGISTER_DATA, buf[j]);
		}

		buf+=256;
	}

	outb(dev->io_base + ATA_REGISTER_COMMAND, ATA_COMMAND_CACHE_FLUSH);
}

uint8_t ata_init(ata_dev_t *dev, bool primary, bool master)
{
	dev->primary = primary;
	dev->master = master;

	dev->io_base = primary ? ATA_PRIMARY : ATA_SECONDARY;

	dev->ready = false;

	// Check for floating bus
	uint8_t status = inb(dev->io_base + ATA_REGISTER_STATUS);
	if (status == 0xFF) {
		ata_device_print(dev, "not connected (floating bus)");
		return ATA_RETURN_NONE;
	}

	outb(dev->primary ? ATA_REGISTER_PRIMARY_CONTROL : ATA_REGISTER_SECONDARY_CONTROL, 0);

	// Select drive
	outb(dev->io_base + ATA_REGISTER_SELECT, master ? 0xA0 : 0xB0);

	// Reset
	outb(dev->io_base + ATA_REGISTER_SECTOR_COUNT, 0);
	outb(dev->io_base + ATA_REGISTER_LBA_LOW, 0);
	outb(dev->io_base + ATA_REGISTER_LBA_MID, 0);
	outb(dev->io_base + ATA_REGISTER_LBA_HIGH, 0);

	// Identify
	outb(dev->io_base + ATA_REGISTER_COMMAND, ATA_COMMAND_IDENTIFY);

	// Check if drive exists
	if (!inb(dev->io_base + ATA_REGISTER_STATUS)) {
		ata_device_print(dev, "not connected");
		return ATA_RETURN_NONE;
	}

	if (ata_pio_wait_bsy(dev) != ATA_RETURN_SUCCESS) {
		return ATA_RETURN_ERROR;
	}

	// Wait until all data is transfered
	if (ata_pio_wait_drq(dev) != ATA_RETURN_SUCCESS) {
		return ATA_RETURN_ERROR;
	}

	// Read identify data
	for (int i=0; i < 256; i++) {
		uint16_t data = inw(dev->io_base + ATA_REGISTER_DATA);

		dev->info[i * 2] = (data >> 8);
		dev->info[i * 2 + 1] = data & 0xFF;
	}

	memcpy(dev->serial, &dev->info[ATA_IDENTIFY_SERIAL], 20);
	dev->serial[19] = '\0';

	memcpy(dev->model, &dev->info[ATA_IDENTIFY_MODEL], 40);
	dev->model[39] = '\0';

	ata_device_print(dev, "ready");

	dev->ready = true;

	return 1;
}