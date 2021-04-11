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
#include "../include/drivers/sb16.h"
#include "../include/drivers/pit.h"
#include "../include/kernel/asm.h"

#include "../bin/shell/shell.h"
#include "../bin/desktop/desktop.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

void kmain(unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;
	mbi = (multiboot_info_t *) addr;

	int debug_port;

	// Init serial debug

	serial_init(DEBUG_PORT);

	kdebug("Multiboot Magic: ");

	if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
		kdebug("Correct!\r\n");
	} else {
		kdebug("Not correct!\r\n");
	}

	void *fb = (void *) (unsigned long) mbi->framebuffer_addr;

	kdebug("-------- Stage 1 --------\r\n");

	kdebug("GDT init\r\n");
	gdt_setup();
	kdebug("IDT init\r\n");
	idt_install();

	kdebug("-------- Stage 2 --------\r\n");

	keyboard_init();
	mouse_init();

	pci_probe();
	if(sb16_probe() == 0) {
		sb16_init();
	}

	#ifdef DESKTOP
		desktop_init(fb, mbi);
	#endif
}
