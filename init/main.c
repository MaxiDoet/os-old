#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../config.h"
#include "../include/kernel/kernel.h"
#include "../include/drivers/serial.h"
#include "../include/drivers/vgacon.h"
#include "../include/drivers/vga-color.h"
#include "../libc/include/mm.h"
#include "../include/kernel/gdt.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/idt.h"
#include "../include/kernel/multiboot.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/pmm.h"
#include "../include/drivers/pci.h"
#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"
#include "../include/drivers/rtc.h"
#include "../include/drivers/sb16.h"
#include "../include/drivers/pit.h"
#include "../include/drivers/ata.h"
#include "../include/kernel/ext2.h"
#include "../include/kernel/mbr.h"
#include "../include/kernel/asm.h"
#include "../include/kernel/smbios.h"

#include "../bin/desktop/desktop.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

extern const void kernel_start;
extern const void kernel_end;

void serial_irq_handler()
{
	char c = serial_read(DEBUG_PORT);

	switch(c) {
		case 'i':
			irq_print_map();
			break;
		case 'p':
			pmm_info();
			break;
	}
}

void kmain(unsigned long magic, unsigned long mbi_addr)
{
	multiboot_info_t *mbi;
	mbi = (multiboot_info_t *) mbi_addr;

	int debug_port;

	// Init serial debug
	serial_init(DEBUG_PORT);
	kdebug(BUILD_INFO);
	kdebug("\r\n");

	// For debug commands
	irq_install_handler(4, serial_irq_handler);

	kdebug("[kernel\e[0;37m] GDT init\r\n");
	gdt_setup();
	kdebug("[kernel\e[0;37m] IDT init\r\n");
	idt_install();

	kdebug("[kernel] PMM init\r\n");
	//pmm_init(mbi);
	//pmm_alloc();

	size_t heap_start = mbi->mem_upper + 10*1024*1024;
	size_t heap_size = mbi->mem_upper * 1024 - heap_size - 10*1024;
	kdebug("[kernel] Heap init\r\n   Start: %x\r\n", heap_start);
	mm_init(heap_start, heap_size);

	keyboard_init();
	mouse_init();

	// RTC init
	kdebug("[rtc] now: ");
	rtc_print_time_date();

	// Detect system hardware
	kdebug("[kernel] SMBIOS info:\r\n");
	uint8_t *smbios_mem = (uint8_t *) 0xF0000;
	while ((unsigned int) smbios_mem < 0x100000) {
		if (smbios_mem[0] == '_' && smbios_mem[1] == 'S' && smbios_mem[2] == 'M' && smbios_mem[3] == '_') {
			smbios_entry_point *entry_point = (smbios_entry_point *) smbios_mem;

			for (int i=0; i < entry_point->structure_table_entries; i++) {
				// Calculate offset
				size_t offset = entry_point->structure_table_address;

				for (int j=0; j < i; j++) {
					smbios_structure_header *header = (smbios_structure_header *) offset;
					offset += header->length;

					// Find double zero byte
					uint8_t *string_table = (uint8_t *) offset;
					int k;
					for (k=1; string_table[k - 1] != '\0' || string_table[k] != '\0'; k++);
					offset += k + 1;
				}

				smbios_structure_header *header = (smbios_structure_header *) offset;
				switch (header->type) {
					case 0:
						;
						kdebug("        BIOS:   %s %dK\r\n", smbios_get_string(offset, header, 0), 64 * (smbios_get_int(offset, header, 8) - 1));
						break;
					case 1:
						;
						kdebug("        Vendor: %s\r\n", smbios_get_string(offset, header, 0));
						kdebug("        Model:  %s\r\n", smbios_get_string(offset, header, 1));
						break;
					case 2:
						;
						kdebug("        Board: %s %s\r\n", smbios_get_string(offset, header, 0), smbios_get_string(offset, header, 1));
						break;
					case 4:
						kdebug("        Processor: ");

						int core_count = smbios_get_int(offset, header, 35);
						if (core_count != 0) kdebug("%d Core(s) ", core_count);

						char* manufacturer = smbios_get_string(offset, header, 1);
						if (manufacturer != "") {
							kdebug("%s ", manufacturer);
						} else {
							kdebug("Unknown OEM ");
						}

						/* Note: Add some cpu family codes here */

						int clock = smbios_get_int(offset, header, 18);
						if (clock != 0) kdebug("%d Mhz ", clock);

						kdebug("\r\n");
						break;
					case 16:
						kdebug("        Memory: ");

						int count = smbios_get_int(offset, header, 13);
						if (count != 0) {
							kdebug("\r\n                %dx ", count);
						} else {
							kdebug("\r\n");
							break;
						}

						int use = smbios_get_int(offset, header, 5);
						if (use != 0) {
							switch (use) {
								case 1:
									kdebug("Unknown Memory");
									break;
								case 2:
									kdebug("Unknown Memory");
									break;
								case 3:
									kdebug("System Memory");
									break;
								case 4:
									kdebug("Video Memory");
									break;
								case 5:
									kdebug("Flash Memory");
									break;
								case 6:
									kdebug("NVRAM");
									break;
								case 7:
									kdebug("Cache");
									break;
								default:
									kdebug("Unknown Memory");
									break;
							}

							kdebug(" ");
						}

						int ecc = smbios_get_int(offset, header, 6);
						if (ecc != 0) {
							switch (ecc) {
								case 1:
									kdebug("Unknown ECC");
									break;
								case 2:
									kdebug("Unknown ECC");
									break;
								case 3:
									kdebug("No ECC");
									break;
								case 4:
									kdebug("Parity ECC");
									break;
								case 5:
									kdebug("Single-bit ECC");
									break;
								case 6:
									kdebug("Multi-bit ECC");
									break;
								case 7:
									kdebug("CRC");
									break;
								default:
									kdebug("Unknown ECC");
									break;
							}

							kdebug(" ");
						}

					kdebug("\r\n");
				}
			}
		}

		smbios_mem+=16;
	}

	kdebug("[kernel] ATA init\r\n");
	ata_dev_t root_dev;

	if (ata_init(&root_dev, ATA_PRIMARY_MASTER, true)) {
		kdebug("[kernel] root device: primary master\r\n");
	} else if (ata_init(&root_dev, ATA_PRIMARY_SLAVE, false)) {
		kdebug("[kernel] root device: primary slave\r\n");
	} else if (ata_init(&root_dev, ATA_SECONDARY_MASTER, true)) {
		kdebug("[kernel] root device: secondary master\r\n");
	} else if (ata_init(&root_dev, ATA_SECONDARY_SLAVE, false)) {
		kdebug("[kernel] root device: secondary slave\r\n");
	} else {
        	kpanic("no ata root device");
	}

	// Look for mbr
        uint16_t *mbr_buf = (uint16_t *) malloc(mm, ATA_SECTOR_SIZE);
        ata_pio_read(root_dev, 0, 1, mbr_buf);
        mbr_t *mbr = (mbr_t *) mbr_buf;

	kdebug("MBR\r\nBoot Type Start\r\n---------------\r\n");

        if (mbr->boot_signature[0] == 0x55 && mbr->boot_signature[1] == 0xAA) {
		for (int i=0; i < 4; i++) {
			mbr_table_entry entry = mbr->partition_table[i];
			kdebug("%s %x    %d\r\n", (entry.boot_flag == 0x80) ? " x  " : "    ", entry.type, entry.start_sector);

			if (entry.type == 0x83) {
				ext2_probe(&root_dev, entry);
			}
		}
	}

	pci_probe();
	if(sb16_probe() == 0) {
		sb16_init();
	}

	#ifdef DESKTOP
		void *fb = (void *) (unsigned long) mbi->framebuffer_addr;
		desktop_init(mbi->framebuffer_addr, mbi->framebuffer_width, mbi->framebuffer_height, mbi->framebuffer_pitch);
	#endif
}
