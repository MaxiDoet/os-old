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
	uint16_t *ata_buf = (uint16_t *) malloc(mm, ATA_SECTOR_SIZE);
        ata_pio_read(*ata_dev, 0, 1, ata_buf);

	mbr_t *mbr = (mbr_t *) ata_buf;

	if (mbr->boot_signature[0] == 0x55 && mbr->boot_signature[1] == 0xAA) {
		kdebug("[fs] Found mbr\r\n");
	}

	for (int i=0; i < 4; i++) {
		if (mbr->partition_table[i]->type == 6) {
			mbr_table_entry entry = mbr->partition_table[i];

			uint32_t fat_start = entry->start_sector;
			
		}
	}
}

