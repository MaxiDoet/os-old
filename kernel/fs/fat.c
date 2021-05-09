#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/fat.h"
#include "../include/kernel/kernel.h"
#include "../include/drivers/ata.h"
#include "../libc/include/mm.h"

void fat_probe(ata_dev_t *ata_dev)
{
	// Read first sector
	uint16_t *mbr_buf = (uint16_t *) malloc(mm, ATA_SECTOR_SIZE);
        ata_pio_read(*ata_dev, 0, 1, mbr_buf);

	mbr_t *mbr = (mbr_t *) mbr_buf;

	if (mbr->boot_signature[0] == 0x55 && mbr->boot_signature[1] == 0xAA) {
		kdebug("[fs] Found mbr\r\n");
	}

	for (int i=0; i < 4; i++) {
		if (mbr->partition_table[i].type == 6) {
			uint16_t *fat_buf = (uint16_t *) malloc(mm, ATA_SECTOR_SIZE);

			mbr_table_entry entry = mbr->partition_table[i];

			kdebug("[fs] Partition %d: Start: %d Type: %d\r\n", i+1, entry.start_sector, entry.type);

			uint32_t fat_start = entry.start_sector;
			ata_pio_read(*ata_dev, fat_start, 1, fat_buf);

			fs_fat16_t *fat = (fs_fat16_t *) fat_buf;

			uint32_t root_dir_start = fat_start + fat->sectors_per_fat * fat->fat_copies + 2;
			uint32_t root_dir_size = (fat->max_root_entries * 32) / fat->bytes_per_sector;
			uint32_t data_start = root_dir_start + root_dir_size;
			uint16_t entries_per_sector = fat->bytes_per_sector / 32;
			uint16_t entries_per_cluster = entries_per_sector * fat->sectors_per_cluster;

			uint16_t *root_dir_buf = (uint16_t *) malloc(mm, ATA_SECTOR_SIZE);
			ata_pio_read(*ata_dev, root_dir_start, 1, root_dir_buf);

			fs_fat16_dir_t *root_first = (fs_fat16_dir_t *) root_dir_buf;
			//kdebug("FAT Info:\r\nroot_dir_start: %d\r\nroot_dir_size: %d\r\ndata_start: %d\r\n", root_dir_start, root_dir_size, data_start);

			/*
			char* filename;
			for (int k=0; k < 8; k++) {
				filename[k] = root_first->file_name[k];
			}
			filename[9] = '\0';

			kdebug("[fs] Filename: %x\r\n", root_first->file_name[0]);
			*/

			for (int l=0; l < 256; l++) {
                		kdebug("%x ", root_dir_buf[l]);
        		}
		}
	}
}

/*
void fat_print_mbr_table(mbr_t *mbr)
{
	if (mbr->boot_signature[0] == 0x55 && mbr->boot_signature[1] == 0xAA) {
		for (int i=0; i < 4; i++) {
			kdebug("");
		}
	}
}
*/
