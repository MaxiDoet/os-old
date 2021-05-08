#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/ata.h"
#include "../include/kernel/asm.h"
#include "../include/kernel/kernel.h"

char* ata_device_tree(bool primary, bool master);
void ata_device_debug(bool primary, bool master, char* msg);

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
	ata_device_debug((port_base == 0x1F0 || port_base == 0x1E8), master, "ready");

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

void ata_device_debug(bool primary, bool master, char* msg)
{
	kdebug("[ata] %s: %s\r\n", ata_device_tree(primary, master), msg);
}

void ata_find(ata_dev_t *dev)
{
	// Look for root ata dev

	if (ata_init(dev, ATA_PRIMARY_MASTER, true)) kdebug("[ata] root device: primary master\r\n"); return;
	if (ata_init(dev, ATA_PRIMARY_SLAVE, false)) kdebug("[ata] root device: primary slave\r\n"); return;
	if (ata_init(dev, ATA_SECONDARY_MASTER, true)) kdebug("[ata] root device: secondary master\r\n"); return;
	if (ata_init(dev, ATA_SECONDARY_SLAVE, false)) kdebug("[ata] root device: secondary slave\r\n"); return;

	kpanic("no ata root device");
}

uint8_t ata_pio_wait_bsy(ata_dev_t dev)
{
	uint8_t status;

        while(status & (1 << 7)) {
		// Wait 400ns before reading the status register
		asm volatile ("nop");
            	asm volatile ("nop");
            	asm volatile ("nop");
            	asm volatile ("nop");
            	asm volatile ("nop");

                status = inb(dev.command_port);
        }

	return status;
}

uint8_t ata_pio_wait_drq(ata_dev_t dev)
{
	uint8_t status;

        while(!(status & ((1 << 3) | (1 << 0)))) {
		kdebug("[ata] waiting\r\n");
                status = inb(dev.command_port);
        }

        return status;
}

void ata_pio_read(ata_dev_t dev, uint32_t lba, int sector_count, uint16_t *buf)
{
	// Select sector
	outb(dev.device_select_port, (dev.master ? 0xE0 : 0xF0) | ((lba & 0x0F000000) >> 24));
	outb(dev.error_port, 0);
	outb(dev.sector_count_port, sector_count);
	outb(dev.lba_low_port, lba & 0x000000FF);
	outb(dev.lba_mid_port, (lba & 0x0000FF00) >> 8);
	outb(dev.lba_high_port, (lba & 0x00FF0000) >> 16);
	outb(dev.command_port, 0x20);

	for (int i=0; i < sector_count; i++) {
		ata_pio_wait_drq(dev);

		for (int j=0; j < 256; j++) {

			uint16_t data = inw(dev.data_port);

			buf[j] = data;
		}

		buf+=226;

		/*
		uint16_t data = inw(dev.data_port);
		*(uint16_t *)(buf + i * 2) = data;
		*/
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

void read_sectors_ATA_PIO(uint32_t target_address, uint32_t LBA, uint8_t sector_count)
{
	outb(0x1F0, 0xE0 | ((LBA >>24) & 0xF));
	outb(0x1F2, sector_count);
	outb(0x1F3, (uint8_t) LBA);
	outb(0x1F4, (uint8_t)(LBA >> 8));
	outb(0x1F5, (uint8_t)(LBA >> 16)); 
	outb(0x1F7, 0x20); //Send the read command

	uint16_t *target = (uint16_t*) target_address;

	for (int j =0;j<sector_count;j++)
	{
		for(int i=0;i<256;i++)
			target[i] = inw(0x1F0);
		target+=256;
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
