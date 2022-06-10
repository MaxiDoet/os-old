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

	kdebug("[vfs] gpt uuid: ");
	for (int i=0; i < 16; i++) {
		kdebug("%x", gpt_table_header->guid[i]);
	}
	kdebug("\r\n");
}

uint8_t vfs_read(char* path, uint16_t *buf)
{
	if (!root_dev->ready) return 0;

	return ext2_read_file(root_dev, &root_fs, path, buf);
}
