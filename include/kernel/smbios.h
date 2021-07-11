#ifndef _smbios_h
#define _smbios_h

enum smbios_header_type {
	SMBIOS_HEADER_TYPE_BIOS_INFO = 0,
	SMBIOS_HEADER_TYPE_SYSTEM_INFO = 1,
	SMBIOS_HEADER_TYPE_MAINBOARD_INFO = 2,
	SMBIOS_HEADER_TYPE_CHASIS_INFO = 3,
	SMBIOS_HEADER_TYPE_PROCESSOR_INFO = 4,
	SMBIOS_HEADER_TYPE_CACHE_INFO = 7,
	SMBIOS_HEADER_TYPE_SYSTEM_SLOTS_INFO = 9,
	SMBIOS_HEADER_TYPE_PHYSICAL_MEMORY_ARRAY = 16,
	SMBIOS_HEADER_TYPE_MEMORY_DEVICE_INFO = 17,
	SMBIOS_HEADER_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS = 19,
	SMBIOS_HEADER_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS = 20,
	SMBIOS_HEADER_TYPE_SYSTEM_BOOT_INFO
};

typedef struct smbios_entry_point {
	char smbios_signature[4]; // Should be _SM_
	uint8_t entry_point_table_checksum;
	uint8_t entry_point_table_length;
	uint8_t major_version;
	uint8_t minor_version;
	uint16_t max_size;
	uint8_t entry_point_revision;
	uint8_t formatted_area[5];
	char dmi_signature[5]; // Should be _DMI_
	uint8_t checksum2;
	uint16_t structure_table_length;
	uint32_t structure_table_address;
	uint16_t structure_table_entries;
	uint8_t smbios_bcd_revision;
} __attribute__((packed)) smbios_entry_point;

typedef struct smbios_structure_header {
	uint8_t type;
	uint8_t length;
	uint16_t handle;
} __attribute__((packed)) smbios_structure_header;

smbios_entry_point *smbios_get_entry_point();
int smbios_get_int(size_t offset, smbios_structure_header *header, int index);
char* smbios_get_string(size_t offset, smbios_structure_header *header, int index);

#endif
