#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../libc/include/mm.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/fs/vfs.h"
#include "../include/kernel/fs/gpt.h"
#include "../include/kernel/fs/ext2.h"
#include "../include/drivers/ata.h"

ext2_fs_t root_fs;
ata_dev_t *root_dev;

const uint8_t gpt_signature[8] = {0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54};

void vfs_probe(ata_dev_t *dev)
{
	root_dev = dev;

	if (!root_dev->ready) return;

	/*
	// Look for mbr
	uint16_t *mbr_buf = (uint16_t *) malloc(ATA_SECTOR_SIZE);
	ata_pio_read(root_dev, 0, 1, mbr_buf);
	mbr_t *mbr = (mbr_t *) mbr_buf;

	if (mbr->boot_signature[0] == 0x55 && mbr->boot_signature[1] == 0xAA) {
		for (int i=0; i < 4; i++) {
			mbr_table_entry entry = mbr->partition_table[i];
			if (entry.type == 0x83) {
				ext2_probe(&root_dev, entry, &root_fs);
			}
        }
    }
	*/

	uint16_t *gpt_table_header_buf = (uint16_t *) malloc(ATA_SECTOR_SIZE);
	ata_pio_read(root_dev, 1, 1, gpt_table_header_buf);
	gpt_table_header_t *gpt_table_header = (gpt_table_header_t *) gpt_table_header_buf;

	/* Check GPT signature */
	for (int i=0; i < 8; i++) {
		if (gpt_table_header->signature[i] != gpt_signature[i]) {
			kdebug("[vfs] invalid gpt signature\r\n");
			return;
		}
	}

	kdebug("[vfs] UUID: ");
	for (int i=0; i < 16; i++) {
		kdebug("%x", gpt_table_header->guid[i]);
	}
	kdebug(" | Entries: %d Start: %d End: %d\r\n", gpt_table_header->table_entry_count, gpt_table_header->start_lba, gpt_table_header->last_lba);

	uint8_t sector_count = (gpt_table_header->table_entry_count * gpt_table_header->table_entry_size) / ATA_SECTOR_SIZE + 1;
	
	/*
	gpt_table_entry_t *gpt_table = (gpt_table_entry_t *) malloc(sector_count * ATA_SECTOR_SIZE);
	ata_pio_read(dev, gpt_table_header->table_lba, sector_count, (uint16_t *) gpt_table);
	*/

	uint16_t *gpt_table_buf = (uint16_t *) malloc(sector_count * ATA_SECTOR_SIZE);
	ata_pio_read(dev, gpt_table_header->table_lba, sector_count, gpt_table_buf);
	gpt_table_entry_t *gpt_table = (gpt_table_entry_t *) gpt_table_buf;

	ext2_probe(dev, gpt_table[0], &root_fs);

	/*
	for (int i=0; i < gpt_table_header->table_entry_count; i++) {
		kdebug("[vfs] %d: %d - %d\r\n", i, gpt_table[i].start_lba, gpt_table[i].last_lba);
	}
	*/

	free(gpt_table_buf);
}

uint8_t vfs_read(char* path, uint16_t *buf)
{
	if (!root_dev->ready) return 0;

	return ext2_read_file(root_dev, &root_fs, path, buf);
}
