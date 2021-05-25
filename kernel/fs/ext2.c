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

static uint16_t *root_buf;
static ext2_inode *inode_buf;

void ext2_read_inode(ata_dev_t *dev, ext2_fs_t *fs, uint32_t inode, ext2_inode *buf)
{
	uint32_t bg = (inode - 1) / fs->sb->inodes_per_group;
	uint32_t bg_index = (inode - 1) % fs->sb->inodes_per_group;
	//bg_index = bg_index % fs->inodes_per_block;
	uint32_t block = (bg_index * sizeof(ext2_inode)) / fs->block_size;
	uint32_t block_index = bg_index % fs->inodes_per_block;

	// Read bgdt
	uint16_t *bgdt_buf = (uint16_t *) malloc(mm, fs->block_size);
	ata_pio_read(*dev, fs->start_sector + block_to_sector(2), fs->block_size / ATA_SECTOR_SIZE, bgdt_buf);
	ext2_bg_descriptor *descriptor = (ext2_bg_descriptor *) bgdt_buf;

	// Seek to block group
	for (int i=0; i < bg; i++) {
		descriptor++;
	}

	uint16_t *block_buf = (uint16_t *) malloc(mm, fs->block_size);
	ata_pio_read(*dev, fs->start_sector + block_to_sector(descriptor->inode_table_start) + block_to_sector(block), fs->block_size / ATA_SECTOR_SIZE , block_buf);

	ext2_inode *inode_temp = (ext2_inode *) block_buf;
	for (int j=0; j < block_index; j++) {
		inode_temp++;
	}

	kdebug("----------------------------------------\r\n");

	for (int k=0; k < 64; k++) {
		kdebug("%x ", *((uint16_t *)inode_temp + k));
	}
	kdebug("\r\n");
	kdebug("read_inode: inode: %d type: %x inode_table_start: %d bg: %d bg_index: %d block_index: %d block: %d sector: %d\r\n", inode, inode_temp->type, descriptor->inode_table_start, bg, bg_index, block_index, block, fs->start_sector + block_to_sector(descriptor->inode_table_start) + block_to_sector(block));
	kdebug("read_inode: direct_block_pointer[0]: %d\r\n", inode_temp->direct_block_ptr[0]);
	kdebug("----------------------------------------\r\n");
	memcpy(buf, inode_temp, sizeof(ext2_inode));
}

uint32_t ext2_find_inode(ata_dev_t *dev, ext2_fs_t *fs, char* path)
{
	uint32_t result;
	size_t current = strsplit(path, '/');
	char* filename = (char *) path + 1; // Remove slash
	ext2_dir_entry *dir = (ext2_dir_entry *) malloc(mm, sizeof(ext2_dir_entry));

	if (current == 1) {
		// Root is always inode 2
		ext2_read_inode(dev, fs, 2, inode_buf);
		for (int i=0; i < 12; i++) {
			uint32_t block = inode_buf->direct_block_ptr[i];
			ata_pio_read(*dev, fs->start_sector + block_to_sector(block), fs->block_size / ATA_SECTOR_SIZE, root_buf);
			dir = (ext2_dir_entry *) root_buf;

			while(dir->inode != 0) {
				char* name = (char *) malloc(mm, dir->name_length + 1);
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
		kdebug("Subdirs are not supported yet!");
	}
}

uint8_t ext2_read_file(ata_dev_t *dev, ext2_fs_t *fs, char* path, uint16_t *buf)
{
	uint32_t inode = ext2_find_inode(dev, fs, path);
	if (inode != 0) {
		kdebug("Found file!\r\n");
	} else {
		return -1;
	}

	ext2_read_inode(dev, fs, inode, inode_buf);

	kdebug("read_file: inode: %d\r\n", inode);
	kdebug("read_file: inode_type: %x\r\n", inode_buf->type);

	for (int i=0; i < 12; i++) {
		uint32_t block = inode_buf->direct_block_ptr[i];
		kdebug("read_file: block: %d sector: %d\r\n", block, block_to_sector(block));
		if (block == 0) {
			// EOF
			kdebug("read_file: EOF\r\n");
			return 0;
		}

		uint16_t *block_buf = (uint16_t *) malloc(mm, fs->block_size);
		ata_pio_read(*dev, fs->start_sector + block_to_sector(block), fs->block_size / ATA_SECTOR_SIZE, block_buf);
		memcpy(buf + i*(fs->block_size), block_buf, fs->block_size);
	}

	return 1;
}

uint8_t ext2_probe(ata_dev_t *dev, mbr_table_entry entry, ext2_fs_t *fs)
{
	uint16_t *sb_buf = (uint16_t *) malloc(mm, 1024);

        ata_pio_read(*dev, entry.start_sector + 2, 2, sb_buf);
	ext2_superblock *sb = (ext2_superblock *) sb_buf;
	if (sb->signature != EXT2_SIGNATURE) return -1;

	fs->start_sector = entry.start_sector;
	fs->sb = sb;
	fs->block_size = 1024 << sb->block_size;
	fs->inodes_per_block = fs->block_size / sizeof(ext2_inode);

	uint32_t bg_descriptors_total = sb->blocks_total / sb->blocks_per_group;
	//uint16_t *bg_descriptor_table = (uint16_t *) malloc(mm, fs->block_size);
	//ata_pio_read(*dev, fs->start_sector + block_to_sector(2), fs->block_size / ATA_SECTOR_SIZE, bg_descriptor_table);
	//fs->bgdt = bg_descriptor_table;

	kdebug("ext2 info: sb_start: %d inodes_per_group: %d bgdt_start: %d\r\n", entry.start_sector + 2, sb->inodes_per_group, fs->start_sector + block_to_sector(2));

	// Init buffers
	if (!inode_buf) inode_buf = (ext2_inode *) malloc(mm, sizeof(ext2_inode));
        if (!root_buf) root_buf = (uint16_t *) malloc(mm, fs->block_size);

        for (int i=1; i < 20; i++) {
		ext2_read_inode(dev, fs, i, inode_buf);
        }

	uint16_t *file_buf = (uint16_t *) malloc(mm, 1000);
	uint8_t result = ext2_read_file(dev, fs, "/test.txt", file_buf);
	kdebug("test.txt: %d Content: %s", result, (char *) file_buf);

	/*
	ext2_read_inode(dev, &fs->sb, fs->bgdt, 2, inode_buf);
	for (int i=0; i < 12; i++) {
		uint32_t block = inode_buf->direct_block_ptr[i];
		if (block == 0) break;
		kdebug("Root dbp%d: %d %d\r\n", i, block, fs->start_sector + block_to_sector(block));
		ata_pio_read(*dev, fs->start_sector + block_to_sector(block), fs->block_size / ATA_SECTOR_SIZE, root_buf);

		ext2_dir_entry *dir = (ext2_dir_entry *) root_buf;
		while(dir->inode != 0) {
			char* name = (char *) malloc(mm, dir->name_length + 1);
			memcpy(name, &dir->name_reserved, dir->name_length);
			name[dir->name_length] = '\0';
			if (strcmp(name, "test.txt") == 0) {
				kdebug("Found file! Inode: %d\r\n", dir->inode);
			}

			dir = (ext2_dir_entry *) ((uint32_t) dir + dir->size);
		}

		/*
		if (ext2_read_directory(dev, fs, (ext2_dir_entry *) root_buf, filename)) {
			kdebug("Works!\r\n");
		}
	}
	*/


	return 1;
}
