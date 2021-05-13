#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/ext2.h"
#include "../include/kernel/mbr.h"
#include "../include/drivers/ata.h"
#include "../include/kernel/kernel.h"
#include "../libc/include/mm.h"

/* Note: Add global device structure instead of just ata devices and attach pointer to the specific read/write/flush functions */

uint8_t ext2_probe(ata_dev_t *dev, mbr_table_entry entry)
{
	uint16_t *sb_buf = (uint16_t *) malloc(mm, 1024);
        ata_pio_read(*dev, entry.start_sector + 2, 2, sb_buf);
	ext2_superblock *sb = (ext2_superblock *) sb_buf;
	if (sb->signature != EXT2_SIGNATURE) return -1;

	ext2_bg_descriptor *bg

	kdebug("EXT2: Inodes: %d Blocks: %d\r\n", sb->inodes_total, sb->blocks_total);
}
