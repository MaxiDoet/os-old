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

			//fs_fat16_t *fat = (fs_fat16_t *) fat_buf;

			for (int j=0; j < 256; j++) {
				kdebug("%x ", fat_buf[j]);
			}

			//kdebug("[fs] fat test: %x\r\n", fat->boot_code[0]);
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
