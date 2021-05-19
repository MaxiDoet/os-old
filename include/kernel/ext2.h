#ifndef _ext2_h
#define _ext2_h

#include "../include/drivers/ata.h"
#include "../include/kernel/mbr.h"

#define EXT2_SIGNATURE 0xEF53

typedef struct ext2_superblock {
	uint32_t inodes_total;
	uint32_t blocks_total;
	uint32_t reserved_su; // These are the blocks that are reserved for the superuser
	uint32_t unallocated_blocks;
	uint32_t unallocated_inodes;
	uint32_t superblock;
	uint32_t block_size;
	uint32_t fragment_size;
	uint32_t blocks_per_group;
	uint32_t fragments_per_group;
	uint32_t inodes_per_group;
	uint32_t last_mount;
	uint32_t last_written;
	uint16_t mounted_since_check;
	uint16_t mounts_until_check;
	uint16_t signature; // For ext2 this must be 0xEF53
	uint16_t state;
	uint16_t error_action;
	uint16_t minor_version;
	uint32_t last_check;
	uint32_t check_interval;
	uint32_t os_id;
	uint32_t major_version;
	uint16_t user_use_reserved;
	uint16_t group_use_reserved;
} __attribute__((packed)) ext2_superblock;

typedef struct ext2_bg_descriptor {
	uint32_t block_usage_map_block;
	uint32_t inode_usage_map_block;
	uint32_t inode_table_start;
	uint16_t unallocated_blocks_in_group;
	uint16_t unallocated_inodes_in_group;
	uint16_t directories_in_group;
} __attribute__((packed)) ext2_bg_descriptor;

enum ext2_inode_type {
	EXT2_INODE_TYPE_FIFO = 0x1000,
	EXT2_INODE_TYPE_CHARDEV = 0x2000,
	EXT2_INODE_TYPE_DIRECTORY = 0x4000,
	EXT2_INODE_TYPE_BLOCKDEV = 0x6000,
	EXT2_INODE_TYPE_REGULAR = 0x8000,
	EXT2_INODE_TYPE_SYMBOLIC_LINK = 0xA000,
	EXT2_INODE_TYPE_SOCKET = 0xC000
};

typedef struct ext2_inode {
	uint16_t type;
	uint16_t user_id;
	uint32_t size_low;
	uint32_t last_access;
	uint32_t creation_time;
	uint32_t last_modification;
	uint32_t deletion_time;
	uint16_t group_id;
	uint16_t hard_links_count;
	uint32_t disk_sectors_count;
	uint32_t flags;
	uint32_t os_specific;
	uint32_t direct_block_ptr[12];
	uint32_t indirect_singly_block_ptr;
	uint32_t indirect_doubly_block_ptr;
	uint32_t indirect_triply_block_ptr;
	uint32_t generation_number;
	uint32_t extended_attribute;
	uint32_t size_high;
	uint32_t fragment_block;
	uint8_t reserved[12];
} __attribute__((packed)) ext2_inode;

typedef struct ext2_dir_entry {
	uint32_t inode;
	uint16_t size;
	uint8_t name_length;
	uint8_t type;
	uint8_t name_reserved;
} __attribute__((packed)) ext2_dir_entry;

typedef struct ext2_fs_t {
	uint32_t start_sector;
	ext2_superblock sb;
	uint16_t *bgdt;
} ext2_fs_t;

uint8_t ext2_probe(ata_dev_t *dev, mbr_table_entry entry, ext2_fs_t *fs);
void ext2_read_inode(ata_dev_t *dev, ext2_superblock *sb, uint16_t *bg_descriptor_table_buf, uint32_t inode, ext2_inode *inode_buf);

#endif
