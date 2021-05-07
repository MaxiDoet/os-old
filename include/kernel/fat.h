#ifndef _fat_h
#define _fat_h

#include "../include/drivers/ata.h"

/* Note: Add hidden partition types */

enum mbr_fs_type {
	MBR_FS_TYPE_EMPTY = 0x00,
	MBR_FS_TYPE_FAT12 = 0x01,
	MBR_FS_TYPE_FAT1632 = 0x04, // Size less then 32MB
	MBR_FS_TYPE_EXTENTED = 0x05,
	MBR_FS_TYPE_FAT16 = 0x06,
	MBR_FS_TYPE_NTFS = 0x07, // Could also be exFAT
	MBR_FS_TYPE_FAT32CHS = 0x1B,
	MBR_FS_TYPE_FAT32 = 0x1C
};

typedef struct mbr_table_entry {
	uint8_t boot_flag;
	uint8_t chs_entry_first[3];
	uint8_t type;
	uint8_t chs_entry_last[3];
	uint8_t start_sector[4];
	uint8_t size[4];
} __attribute__((packed)) mbr_table_entry;

typedef struct mbr_t {
	uint8_t bootloader[440];
	uint8_t signature[4];
	uint16_t null;
	mbr_table_entry partition_table[4];
	uint8_t boot_signature[2];
} __attribute__((packed)) mbr_t;

void fat_probe(ata_dev_t *ata_dev);

#endif
