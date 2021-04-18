#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../config.h"
#include "../include/kernel/kernel.h"
#include "../include/drivers/serial.h"
#include "../include/drivers/vgacon.h"
#include "../include/drivers/vga-color.h"
#include "../include/lib/print.h"
#include "../include/lib/convert.h"
#include "../include/kernel/gdt.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/idt.h"
#include "../include/kernel/multiboot.h"
#include "../include/kernel/kernel.h"
#include "../include/drivers/pci.h"
#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"
#include "../include/drivers/cmos.h"
#include "../include/drivers/sb16.h"
#include "../include/drivers/pit.h"
#include "../include/drivers/ata.h"
#include "../include/kernel/asm.h"

#include "../bin/desktop/desktop.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

void kmain(unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;
	mbi = (multiboot_info_t *) addr;

	int debug_port;

	// Init serial debug
	serial_init(DEBUG_PORT);

	kdebug(BUILD_INFO);

	kdebug("-------- Init Stage 1 --------\r\n");

	kdebug("GDT init\r\n");
	gdt_setup();
	kdebug("IDT init\r\n");
	idt_install();

	kdebug("-------- Init Stage 2 --------\r\n");

	keyboard_init();
	mouse_init();

	void *fb = (void *) (unsigned long) mbi->framebuffer_addr;

	pci_probe();
	if(sb16_probe() == 0) {
		sb16_init();
	}

	int8_t data[] = {
		0xEF
	};

	ata_dev_t ata0 = ata_init(0x1F0, true);
	ata_write(ata0, 1, data, 16);
	ata_flush(ata0);
	uint16_t data_read = ata_read(ata0, 1, 16);
	kdebug("Read: %x\r\n", data_read);

	if(data_read == 0xEF) {
		kdebug("it works!\r\n");
	}

	kdebug("-------- Init Stage 3 --------\r\n");

	#ifdef DESKTOP
		desktop_init(mbi->framebuffer_addr, mbi->framebuffer_width, mbi->framebuffer_height, mbi->framebuffer_pitch);
	#endif
}
