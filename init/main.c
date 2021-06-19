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
#include "../include/kernel/vfs.h"
#include "../include/kernel/io.h"
#include "../include/kernel/smbios.h"

#include "../apps/desktop/desktop.h"

#include "../lib/include/libbmp.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

extern const void kernel_start;
extern const void kernel_end;

extern void enable_sse;

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

	//kdebug("[kernel] PMM init\r\n");
	//pmm_init(mbi);
	//pmm_alloc();

	size_t heap_start = mbi->mem_upper + 10*1024*1024;
	//size_t heap_size = mbi->mem_upper * 1024 - heap_size - 10*1024;
	size_t heap_size = 1000000;
	kdebug("[kernel] Heap init\r\n   Start: %x\r\n", heap_start);
	mm_init(heap_start, heap_size);

	// RTC init
	kdebug("[rtc] now: ");
	rtc_print_time_date();

	// Detect system hardware
	kdebug("[kernel] SMBIOS Version: ");
	uint8_t *smbios_mem = (uint8_t *) 0xF0000;
	while ((unsigned int) smbios_mem < 0x100000) {
		if (smbios_mem[0] == '_' && smbios_mem[1] == 'S' && smbios_mem[2] == 'M' && smbios_mem[3] == '_') {
			smbios_entry_point *entry_point = (smbios_entry_point *) smbios_mem;
			kdebug("%d.%d\r\n", entry_point->major_version, entry_point->minor_version);
			break;
		}
		smbios_mem+=16;
	}
	if ((unsigned int) smbios_mem == 0x100000) {
		kdebug("not present\r\n");
	}

	/* Doesn't work on copy.sh!
	if(sb16_probe() == 0) {
		sb16_init();
	}
	*/

	kdebug("[kernel] ATA init\r\n");
	ata_dev_t root_dev;
	ext2_fs_t fs;

	if (ata_init(&root_dev, ATA_PRIMARY_MASTER, true)) {
		kdebug("[kernel] root device: %s\r\n", ata_device_tree(true, true));
	} else if (ata_init(&root_dev, ATA_PRIMARY_SLAVE, false)) {
		kdebug("[kernel] root device: %s\r\n", ata_device_tree(true, false));
	} else if (ata_init(&root_dev, ATA_SECONDARY_MASTER, true)) {
		kdebug("[kernel] root device: %s\r\n", ata_device_tree(false, true));
	} else if (ata_init(&root_dev, ATA_SECONDARY_SLAVE, false)) {
		kdebug("[kernel] root device: %s\r\n", ata_device_tree(false, false));
	} else {
        	kpanic("no ata root device");
	}

	vfs_probe(root_dev);

	uint16_t *file_buf = (uint16_t *) malloc(mm, 1000);
        vfs_read("/audio.wav", file_buf);

	//kdebug("\e[0;33mTest\r\n");

	pci_scan();

	keyboard_init();
	mouse_init();

	#ifdef DESKTOP
		void *fb = (void *) (unsigned long) mbi->framebuffer_addr;
		desktop_init(mbi->framebuffer_addr, mbi->framebuffer_width, mbi->framebuffer_height, mbi->framebuffer_pitch);
	#endif
}
