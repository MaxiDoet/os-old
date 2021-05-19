#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/ext2.h"
#include "../include/kernel/mbr.h"
#include "../include/drivers/ata.h"
#include "../include/kernel/kernel.h"
#include "../libc/include/mm.h"
#include "../libc/include/string.h"

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

void ext2_read_inode(ata_dev_t *dev, ext2_superblock *sb, uint16_t *bg_descriptor_table_buf, uint32_t inode, ext2_inode *inode_buf)
{
	uint32_t bg = (inode - 1) / sb->inodes_per_group;
	uint32_t bg_index = (inode - 1) % sb->inodes_per_group;
	uint32_t block = (bg_index * 128) / 1024;
	ext2_bg_descriptor *descriptor = (ext2_bg_descriptor *) bg_descriptor_table_buf + bg;

	uint16_t *block_buf = (uint16_t *) malloc(mm, 1024);
	kdebug("table_start: %d bg: %d bg_index: %d block: %d sector: %d\r\n", descriptor->inode_table_start, bg, bg_index, block, 2248 + block_to_sector(block));
	ata_pio_read(*dev, 2248 + block_to_sector(block), 2, block_buf);

	ext2_inode *inode_temp = (ext2_inode *) block_buf;
	for (int i=0; i < bg_index; i++) {
		inode_temp++;
	}

	memcpy(inode_buf, inode_temp, sizeof(ext2_inode));
}

void ext2_read_root(ata_dev_t *dev, ext2_fs_t *fs)
{
	// Read root dir
	ext2_inode *inode = (ext2_inode *) malloc(mm, sizeof(ext2_inode));
	ext2_read_inode(dev, &fs->sb, fs->bgdt, 2, inode);

	for (int i=0; i < 12; i++) {
		uint32_t block = inode->direct_block_ptr[i];
		if (block == 0) break;
		kdebug("Root dbp: %d\r\n", fs->start_sector + block_to_sector(block));
	}

}

uint8_t ext2_probe(ata_dev_t *dev, mbr_table_entry entry, ext2_fs_t *fs)
{
	uint16_t *sb_buf = (uint16_t *) malloc(mm, 1024);

        ata_pio_read(*dev, entry.start_sector + 2, 2, sb_buf);
	ext2_superblock *sb = (ext2_superblock *) sb_buf;
	if (sb->signature != EXT2_SIGNATURE) return -1;

	kdebug("%d\r\n", sb->blocks_total);

	uint32_t bg_descriptors_total = sb->blocks_total / sb->blocks_per_group;
	kdebug("Block Size: %d\r\n", 1024 << sb->block_size);

	uint16_t *bg_descriptor_table = (uint16_t *) malloc(mm, 512);
	ata_pio_read(*dev, entry.start_sector + block_to_sector(2), 1, bg_descriptor_table);
	ext2_bg_descriptor *bg_descriptor_first = (ext2_bg_descriptor *) bg_descriptor_table;

	kdebug("Test: %d\r\n", entry.start_sector + block_to_sector(bg_descriptor_first->inode_table_start));

	fs->start_sector = entry.start_sector;
	fs->sb = *sb;
	fs->bgdt = bg_descriptor_table;

	ext2_read_root(dev, fs);

	return 1;
}
