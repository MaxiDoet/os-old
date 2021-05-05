#ifndef _fat16_h
#define _fat16_h

typedef struct fat16_bpb {
	uint8_t boot_code[3];
	uint16_t oem_name[4];
	uint16_t sector_size;
	uint8_t cluster_size_in_sectors;
	uint16_t reserved_sectors;
	uint8_t fat_count;
	uint16_t root_max_entries;
	uint16_t sector_count;
	uint8_t media_descriptor;
	uint16_t fat_size_in_sectors;
	uint16_t track_size_in_sectors;
	uint16_t read_write_heads;
	uint32_t sectors_before_boot;
	uint16_t sector_count_alt;
} __attribute__((__packed__)) fat16_bpb;

#define FAT16_SECTORS_PER_CLUSTER 1


#endif
