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

uint8_t ext2_probe(ata_dev_t *dev, mbr_table_entry entry);

#endif
