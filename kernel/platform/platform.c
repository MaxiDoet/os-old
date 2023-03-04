#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "../include/kernel/platform.h"
#include "../include/kernel/smbios.h"
#include "../include/kernel/kernel.h"

bool is_qemu=false;

void platform_init()
{
	smbios_entry_point *entry_point = smbios_get_entry_point();

	if (entry_point && entry_point->major_version) {
		kdebug("[platform] SMBIOS Version: %d.%d\r\n", entry_point->major_version, entry_point->minor_version);

		size_t offset = entry_point->structure_table_address;

		for (uint16_t i=0; i < entry_point->structure_table_entries; i++) {
			for (uint16_t j=0; j < i; j++) {
				smbios_structure_header *header = (smbios_structure_header *) offset;
				offset += header->length;

				// Find double zero byte
				uint8_t *string_table = (uint8_t *) offset;
				int k;
				for (k=1; string_table[k - 1] != '\0' || string_table[k] != '\0'; k++)

				offset += k + 1;
			}

			smbios_structure_header *header = (smbios_structure_header *) offset;

			if (!header->length) {
				continue;
			}

			switch((enum smbios_header_type) header->type) {
				case SMBIOS_HEADER_TYPE_BIOS_INFO:
					kdebug("[platform] Bios | %s | %s | %s\r\n", smbios_get_string(offset, header, 0), smbios_get_string(offset, header, 1), smbios_get_string(offset, header, 2));

					break;
			}
		}
	}
}


bool platform_is_qemu()
{
	return is_qemu;
}
