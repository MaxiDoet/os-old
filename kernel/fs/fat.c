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
			fs_fat16_dir_t *root_last = (fs_fat16_dir_t *) root_dir_buf + 10;
			kdebug("FAT Info:\r\nsectors_per_cluster: %d\r\nroot_dir_size: %d\r\ndata_start: %d\r\n", fat->sectors_per_cluster, root_dir_start, root_dir_size, data_start);

			fs_fat16_dir_t *test = (fs_fat16_dir_t *) root_dir_buf + 7;
			uint32_t test_sector = data_start + (test->start_cluster - 2) * 2;

			kdebug("test:\r\nName: %s\r\nStart cluster: %d\r\nSector: %d\r\n", (char *) test->file_name, test->start_cluster, test_sector);

			uint16_t *test_buf = (uint16_t *) malloc(mm, test->size);
			ata_pio_read(*ata_dev, test_sector, 1, test_buf);

			for (int l=0; l < 256; l++) {
				char* text = "  \0";
				text[1] = (test_buf[l] >> 8) & 0xFF;
				text[0] = test_buf[l] & 0xFF;
				kdebug(text);
			}

			/*
			kdebug("D Filename    First cluster \r\n");
			for (fs_fat16_dir_t *dir = root_first; dir != root_last; ++dir) {
				if (dir->attribute & FAT_ATTRIBUTE_SUBDIR) {
					kdebug("x ");
				} else {
					kdebug("  ");
				}

				kdebug((char *) dir->file_name);

				kdebug("%d\r\n", dir->start_cluster);
			}
			*/

			/*
			for (int l=0; l < 256; l++) {
                		kdebug("%x ", root_dir_buf[l]);
        		}
			*/
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
