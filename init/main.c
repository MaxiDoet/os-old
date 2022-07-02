#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../config.h"
#include "../include/kernel/kernel.h"
#include "../include/drivers/serial.h"
#include "../libc/include/mm.h"
#include "../include/kernel/gdt.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/idt.h"
#include "../include/kernel/multiboot.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/pmm.h"
#include "../include/kernel/console.h"
#include "../include/drivers/pci.h"
#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"
#include "../include/drivers/rtc.h"
#include "../include/drivers/pit.h"
#include "../include/drivers/ata.h"
#include "../include/drivers/ac97.h"
#include "../include/drivers/rtl8139.h"
#include "../include/kernel/fs/ext2.h"
#include "../include/kernel/fs/mbr.h"
#include "../include/kernel/fs/vfs.h"
#include "../include/kernel/io.h"
#include "../include/kernel/platform.h"
#include "../libc/include/string.h"

#include "../apps/desktop/desktop.h"

#include "../include/lib/wav.h"

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

	kdebug("[kernel\e[0;37m] GDT init\r\n");
    gdt_setup();
    kdebug("[kernel\e[0;37m] IDT init\r\n");
    idt_install();

	// Init serial debug
	serial_init(DEBUG_PORT);
	kdebug(BUILD_INFO);
	kdebug("\r\n");

	// For debug commands
	irq_install_handler(4, serial_irq_handler);

	// Init PIT
	pit_init();

	//kdebug("[kernel] PMM init\r\n");
	//pmm_init(mbi);
	//pmm_alloc();

	size_t heap_start = mbi->mem_upper + 10*1024*1024;
	//size_t heap_size = mbi->mem_upper * 1024 - heap_size - 10*1024;
	size_t heap_size = 1000000000;
	kdebug("[kernel] Heap init\r\n   Start: %x\r\n", heap_start);
	mm_init(heap_start, heap_size);

	kdebug("[kernel] Platform init\r\n");
	platform_init();

	kdebug("[kernel] ATA init\r\n");
	ata_dev_t root_dev;
	ext2_fs_t fs;

	if (ata_init(&root_dev, true, true)) {
		kdebug("[kernel] root device: ATA->Primary Master\r\n");
	} else if (ata_init(&root_dev, true, false)) {
		kdebug("[kernel] root device: ATA->Primary Slave\r\n");
	} else if (ata_init(&root_dev, false, true)) {
		kdebug("[kernel] root device: ATA->Secondary Master\r\n");
	} else if (ata_init(&root_dev, false, false)) {
		kdebug("[kernel] root device: ATA->Secondary Slave\r\n");
	}

	if (root_dev.ready) {
		vfs_probe(&root_dev);
	}

	pci_scan();

	keyboard_init();
	mouse_init();

	desktop_init(mbi);
}
