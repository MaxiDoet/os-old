#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/fs/ext2.h"
#include "../include/kernel/fs/gpt.h"
#include "../include/drivers/ata.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/mem/heap.h"
#include "../libc/include/string.h"
#include "../config.h"

uint32_t block_to_sector(uint32_t block)
{
	uint32_t sector = (block * 1024) / ATA_SECTOR_SIZE;

	return sector;
}

static uint8_t *root_buf;

void ext2_read_block(ata_dev_t *dev, ext2_fs_t *fs, uint32_t block, uint8_t *buf)
{
	if (block > fs->sb->blocks_total) {
		kdebug("[ext2] attempted to read invalid block\r\n");
	}

	#ifdef DEBUG_EXT2
	kdebug("[ext2] reading block %d (sector %d)\r\n", block, fs->start_sector + block_to_sector(block));
	#endif

	ata_pio_read(dev, fs->start_sector + block_to_sector(block), fs->block_size / ATA_SECTOR_SIZE, (uint16_t *) buf);
}

void ext2_read_singly_linked(ata_dev_t *dev, ext2_fs_t *fs, uint32_t block_ptr, uint8_t *buf)
{
	uint16_t entries_max = fs->block_size / sizeof(uint32_t);

	uint8_t *block_list_buf = (uint8_t *) malloc(fs->block_size);
	ext2_read_block(dev, fs, block_ptr, block_list_buf);
	uint32_t *block = (uint32_t *) block_list_buf;

	for (int i=0; i < entries_max; i++) {
		if (block[i] == 0) {
			// EOF
			break;
		}

		uint8_t *block_buf = (uint8_t *) malloc(fs->block_size);
		ext2_read_block(dev, fs, block[i], block_buf);
		memcpy(buf + (i * fs->block_size), block_buf, fs->block_size);
		free(block_buf);
	}

	free(block_list_buf);
}

void ext2_read_doubly_linked(ata_dev_t *dev, ext2_fs_t *fs, uint32_t block_ptr, uint8_t *buf)
{
	uint16_t entries_max = fs->block_size / sizeof(uint32_t);

	uint8_t *block_list_buf = (uint8_t *) malloc(fs->block_size);
	ext2_read_block(dev, fs, block_ptr, block_list_buf);
	uint32_t *block = (uint32_t *) block_list_buf;

	for (int i=0; i < entries_max; i++) {
		if (block[i] == 0) {
			// EOF
			break;
		}

		ext2_read_singly_linked(dev, fs, block[i], buf + i * (fs->block_size * fs->block_size / 4));
	}

	free(block_list_buf);
}

void ext2_read_inode(ata_dev_t *dev, ext2_fs_t *fs, uint32_t inode, ext2_inode *buf)
{
	if (inode > fs->sb->inodes_total) {
		kdebug("[ext2] attempted to read invalid inode\r\n");
		return;
	}

	uint32_t bg = (inode - 1) / fs->sb->inodes_per_group;
	uint32_t bg_index = (inode - 1) % fs->sb->inodes_per_group;
	uint32_t block = (bg_index * fs->inode_size) / fs->block_size;
	uint32_t block_index = bg_index % fs->inodes_per_block;

	ext2_bg_descriptor *bg_descriptor = &fs->bgdt[bg];

	uint8_t *block_buf = (uint8_t *) malloc(fs->block_size);
	ext2_read_block(dev, fs, bg_descriptor->inode_table_start + block, block_buf);

	ext2_inode *inode_temp = (ext2_inode *) block_buf;
	memcpy(buf, &inode_temp[block_index], sizeof(ext2_inode));

	free(block_buf);
}

uint32_t ext2_find_inode(ata_dev_t *dev, ext2_fs_t *fs, char* path)
{
	uint32_t result;
	size_t current = strsplit(path, '/');
	char* filename = (char *) path + 1; // Remove slash
	ext2_dir_entry *dir = (ext2_dir_entry *) malloc(sizeof(ext2_dir_entry));
	ext2_inode *inode_buf = (ext2_inode *) malloc(sizeof(ext2_inode));

	if (current == 1) {
		// Root is always inode 2
		ext2_read_inode(dev, fs, 2, inode_buf);
		
		for (int i=0; i < 12; i++) {
			uint32_t block = inode_buf->direct_block_ptr[i];
			ext2_read_block(dev, fs, block, root_buf);
			dir = (ext2_dir_entry *) root_buf;

			while(dir->inode != 0) {
				char* name = (char *) malloc(dir->name_length + 1);
				memcpy(name, &dir->name_reserved, dir->name_length);
				name[dir->name_length] = '\0';
				if (strcmp(name, filename) == 0) {
					return dir->inode;
				}
				
				dir = (ext2_dir_entry *) ((uint32_t) dir + dir->size);
			}
		}

		return 0;
	} else {
		kdebug("[ext2] Subdirs are not supported yet!");
	}

	free(inode_buf);
	free(dir);
}

uint8_t ext2_read_file(ata_dev_t *dev, ext2_fs_t *fs, char* path, uint8_t *buf)
{
	uint32_t inode = ext2_find_inode(dev, fs, path);
	if (inode == 0) return -1;

	ext2_inode *inode_buf = (ext2_inode *) malloc(sizeof(ext2_inode));
	ext2_read_inode(dev, fs, inode, inode_buf);

	#ifdef DEBUG_EXT2
	kdebug("[ext2] read_file: inode: %d type: %x\r\n", inode, inode_buf->type);
	#endif

	for (int i=0; i < 12; i++) {
		uint32_t block = inode_buf->direct_block_ptr[i];
		if (block == 0) {
			// EOF
			return 0;
		}

		uint8_t *block_buf = (uint8_t *) malloc(fs->block_size);
		ext2_read_block(dev, fs, block, block_buf);
		memcpy(buf + (i * fs->block_size), block_buf, fs->block_size);
		free(block_buf);
	}

	if (inode_buf->indirect_singly_block_ptr != 0) {
		ext2_read_singly_linked(dev, fs, inode_buf->indirect_singly_block_ptr, buf + (12 * fs->block_size));
	}

	if (inode_buf->indirect_doubly_block_ptr != 0) {
		ext2_read_doubly_linked(dev, fs, inode_buf->indirect_doubly_block_ptr, buf + (12 * fs->block_size) + (fs->block_size * fs->block_size / 4));
	}

	free(inode_buf);

	return 1;
}

uint8_t ext2_probe(ata_dev_t *dev, gpt_table_entry_t entry, ext2_fs_t *fs)
{
	uint16_t *sb_buf = (uint16_t *) malloc(1024);
    ata_pio_read(dev, entry.start_lba + 2, 2, sb_buf);
	
	ext2_superblock *sb = (ext2_superblock *) sb_buf;
	if (sb->signature != EXT2_SIGNATURE) return -1;

	fs->start_sector = entry.start_lba;
	fs->sb = sb;

	fs->inode_size = 128;

	if (sb->major_version >= 1) {
		kdebug("[ext2] Using extended superblock\r\n");

		fs->inode_size = sb->inode_size;
	}

	fs->block_size = (1024 << sb->block_size);
	fs->inodes_per_block = fs->block_size / fs->inode_size;
	fs->block_groups_total = sb->blocks_total / sb->blocks_per_group;

	kdebug("[ext2] Block size: %d Inode size: %d\r\n", fs->block_size, fs->inode_size);

	/* Read block group descriptor table */
	uint8_t sector_count = (sizeof(ext2_bg_descriptor) * fs->block_groups_total) / ATA_SECTOR_SIZE + 1;
	fs->bgdt = (ext2_bg_descriptor *) malloc(sector_count * ATA_SECTOR_SIZE);
	ata_pio_read(dev, entry.start_lba + block_to_sector(2), sector_count, (uint16_t *) fs->bgdt);

	// Init buffers
    if (!root_buf) root_buf = (uint8_t *) malloc(fs->block_size);

	return 1;
}
