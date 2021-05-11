#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/fat.h"
#include "../include/kernel/kernel.h"
#include "../include/drivers/ata.h"
#include "../libc/include/mm.h"

char* fat_get_filename(fs_fat16_dir_t *dir);
uint32_t fat_cluster_to_sector(uint32_t data_start, uint16_t cluster);

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
			mbr_table_entry entry = mbr->partition_table[i];
			kdebug("[fs] Partition %d: Start: %d Type: %d\r\n", i+1, entry.start_sector, entry.type);

			uint32_t fat_start = entry.start_sector;
			uint16_t *fat_buf = (uint16_t *) malloc(mm, ATA_SECTOR_SIZE);
			ata_pio_read(*ata_dev, fat_start, 1, fat_buf);
			fs_fat16_info *fat = (fs_fat16_info *) fat_buf;

			uint32_t root_dir_start = fat_start + fat->sectors_per_fat * fat->fat_copies + 2;
			uint32_t root_dir_size = (fat->max_root_entries * 32) / fat->bytes_per_sector;
			uint32_t data_start = root_dir_start + root_dir_size;
			uint16_t entries_per_sector = fat->bytes_per_sector / 32;
			uint16_t entries_per_cluster = entries_per_sector * fat->sectors_per_cluster;

			uint16_t *root_dir_buf = (uint16_t *) malloc(mm, ATA_SECTOR_SIZE * root_dir_size);
			ata_pio_read(*ata_dev, root_dir_start, root_dir_size, root_dir_buf);

			fs_fat16_dir_t *root_first = (fs_fat16_dir_t *) root_dir_buf;
			fs_fat16_dir_t *root_last = (fs_fat16_dir_t *) root_dir_buf + root_dir_size;

			fs_fat16_dir_t *test = (fs_fat16_dir_t *) root_dir_buf + 7;
			uint32_t test_sector = 2284;
			uint16_t *test_buf = (uint16_t *) malloc(mm, test->size);

			ata_pio_read(*ata_dev, test_sector, 1, test_buf);

			kdebug("root: %x test: %x\r\n", *root_dir_buf, *test_buf);

			/*
			fs_fat16_dir_t *folder = (fs_fat16_dir_t *) root_dir_buf + 2;
			kdebug("rs: %d ds: %d folder: %d rds: %d\r\n", root_dir_start, data_start, fat_cluster_to_sector(data_start, folder->start_cluster), root_dir_size);

			for (int l=0; l < 256; l++) {
				char* text = "  \0";
				text[1] = (test_buf[l] >> 8) & 0xFF;
				text[0] = test_buf[l] & 0xFF;
				kdebug(text);
			}
			*/

			for (int k=0; k < 256; k++) {
                		kdebug("%x ", root_dir_buf[k]);
        		}
		}
	}
}

char* fat_get_filename(fs_fat16_dir_t *dir)
{
	char* filename = (char *) dir->file_name;
	char* extension = (char *) dir->file_extension;
	int i;

	// Remove tail spaces from basename
	for (i = 7; i != 0; i--) {
		if (filename[i] != ' ') break;
		filename[i] == '\0';
	}

	for (int j=0; j < 2; j++) {
		filename[i+j] = extension[j];
	}

	return filename;
}

uint32_t fat_cluster_to_sector(uint32_t data_start, uint16_t cluster)
{
	if (cluster <= 1) return 0;

	uint32_t sector = data_start + (cluster - 2) * 2;
	return sector;
}
