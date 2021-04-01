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
#include "../bin/shell/shell.h"
#include "../include/kernel/gdt.h"
#include "../include/kernel/idt.h"
#include "../include/kernel/multiboot.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

void putpixel(void *fb, multiboot_info_t *mbi, int x, int y, int color)
{
	multiboot_uint16_t *pixel = fb + mbi->framebuffer_pitch * y + 2 * x;
        *pixel = color;
}

void drawrect(void *fb, multiboot_info_t *mbi, int x, int y, int w, int h, int color)
{
	int i,j;

	for (i=0; i < h; i++) {
		for (j=0; j < w; j++) {
			putpixel(fb, mbi, x+j, y+i, color);
		}
	}
}

void kmain(unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;
	mbi = (multiboot_info_t *) addr;

	int debug_port;

	// Init serial debug
	#ifdef DEBUG_COM1
		debug_port = 0x3F8;
	#elif DEBUG_COM2
		debug_port = 0x2F8;
	#elif DEBUG_COM3
		debug_port = 0x3E8;
	#elif DEBUG_COM4
		debug_port = 0x2E8;
	#else
		debug_port = 0x3F8;
	#endif

	serial_init(debug_port);

	kdebug(debug_port, "Multiboot Magic: ");

	if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
		kdebug(debug_port, "Correct!");
	} else {
		kdebug(debug_port, "Not correct!");
	}

	kdebug(debug_port, "\r\n");

	// VBE Info
	//kdebug(debug_port, "\r\n");

	multiboot_uint32_t color;
	unsigned i;
	void *fb = (void *) (unsigned long) mbi->framebuffer_addr;

	// A little desktop

	drawrect(fb, mbi, 20, 50, 200, 50, 0xffffff);

	/*
	vgacon_init();

	vgacon_clear(VGA_COLOR_BLACK);

	vgacon_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

	prints("Welcome!\n\n");

	prints("gdt init\n");
	gdt_setup();
	prints("idt init\n");
	idt_setup();

	shell_main();
	*/

	//putpixel(fb, 1, 1, 0x7800);
}

void keyboard_irq_handler()
{
	prints("Keyboard IRQ");
}
