#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/ext2.h"
#include "../include/kernel/mbr.h"
#include "../include/drivers/ata.h"
#include "../include/kernel/kernel.h"
#include "../libc/include/mm.h"

/* Note: Add global device structure instead of just ata devices and attach pointer to the specific read/write/flush functions

dev->fs
	fs->probe (void *)
	fs->read (void *)
	.....
	fs->data contains fs specific data e.g fs->data->inodes_per_block
*/

uint32_t block_to_sector(uint32_t block)
{
	uint32_t sector = (block * 1024) / ATA_SECTOR_SIZE;

	return sector;
}

uint32_t ext2_read_inode(ext2_superblock *sb, uint16_t *bg_descriptor_table_buf, uint32_t inode)
{
	uint32_t bg = (inode - 1) / sb->inodes_per_group;
	uint32_t bg_index = (inode - 1) % sb->inodes_per_group;
	uint32_t block = (bg_index * 128) / sb->block_size;
	ext2_bg_descriptor *descriptor = (ext2_bg_descriptor *) bg_descriptor_table_buf + bg;

	
}

uint8_t ext2_probe(ata_dev_t *dev, mbr_table_entry entry)
{
	uint16_t *sb_buf = (uint16_t *) malloc(mm, 1024);

        ata_pio_read(*dev, entry.start_sector + 2, 2, sb_buf);
	ext2_superblock *sb = (ext2_superblock *) sb_buf;
	if (sb->signature != EXT2_SIGNATURE) return -1;

	kdebug("%d\r\n", sb->blocks_total);

	uint32_t bg_descriptors_total = sb->blocks_total / sb->blocks_per_group;
	kdebug("Test: bgdt: %d bgdtf: %d\r\n", bg_descriptors_total, entry.start_sector + block_to_sector(2));

	uint16_t *bg_descriptor_table_buf = (uint16_t *) malloc(mm, 512);
	ata_pio_read(*dev, 2052, 1, bg_descriptor_table_buf);
	ext2_bg_descriptor *bg_descriptor_first = (ext2_bg_descriptor *) bg_descriptor_table_buf;

	kdebug("Test: %d\r\n", entry.start_sector + block_to_sector(bg_descriptor_first->inode_table_start));

	return 1;
}
