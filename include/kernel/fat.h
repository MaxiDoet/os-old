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
	uint32_t start_sector;
	uint32_t size;
} __attribute__((packed)) mbr_table_entry;

typedef struct mbr_t {
	uint8_t bootloader[440];
	uint8_t signature[4];
	uint16_t null;
	mbr_table_entry partition_table[4];
	uint8_t boot_signature[2];
} __attribute__((packed)) mbr_t;

typedef struct fs_fat16_t {
	uint8_t boot_code[3];
	uint8_t oem_name[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t fat_copies;
	uint16_t max_root_entries;
	uint16_t sector_count_alt;
	uint8_t media_descriptor;
	uint16_t sectors_per_fat;
	uint16_t sectors_per_track;
	uint16_t read_write_heads;
	uint32_t hidden_sectors;
	uint16_t sector_count;
	uint8_t bios_drive_number;
	uint8_t reserved;
	uint8_t extended_boot_signature;
	uint8_t fs_id[4];
	uint8_t fs_name[11];
	uint8_t fat_type[8];
	uint8_t bootloader[448];
	uint8_t boot_signature[2];
} __attribute__((packed)) fs_fat16_t;

typedef struct fs_fat16_entry {
	uint8_t file_name[8];
	uint8_t file_extension[3];
	uint8_t attribute;
	uint8_t reserved[10];
	uint16_t time_last_change;
	uint16_t date_last_change;
	uint16_t start_cluster;
	uint32_t size;
} __attribute__((packed)) fs_fat16_entry;

void fat_probe(ata_dev_t *ata_dev);

#endif
