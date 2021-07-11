#include <stdint.h>
#include <stdbool.h>

#include "../include/kernel/smbios.h"

smbios_entry_point *smbios_get_entry_point()
{
	uint8_t *smbios_mem = (uint8_t *) 0xF0000;
	smbios_entry_point *entry_point;

	while((unsigned int) smbios_mem < 0x100000) {
		if (smbios_mem[0] == '_' && smbios_mem[1] == 'S' && smbios_mem[2] == 'M' && smbios_mem[3] == '_') {
			entry_point = (smbios_entry_point *) smbios_mem;
			return entry_point;
		}

		smbios_mem+=16;
	}

	return entry_point;
}

uint8_t *smbios_mem = (uint8_t *) 0xF0000;
        while ((unsigned int) smbios_mem < 0x100000) {
                if (smbios_mem[0] == '_' && smbios_mem[1] == 'S' && smbios_mem[2] == 'M' && smbios_mem[3] == '_') {
                        smbios_entry_point *entry_point = (smbios_entry_point *) smbios_mem;

int smbios_get_int(size_t offset, smbios_structure_header *header, int index)
{
	uint8_t *p = (uint8_t *) offset + index;

	return *p;
}

char* smbios_get_string(size_t offset, smbios_structure_header *header, int index)
{
	uint8_t *string_table = (uint8_t *) offset + header->length;
	char* s = (char *) string_table;

	for (int i=0; i < index; i++) {
		while (*s != '\0') {
			s++;
		}

		s++;
	}

	return s;
}
