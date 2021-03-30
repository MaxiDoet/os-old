#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/vgacon.h"
#include "../include/vga-color.h"
#include "../include/print.h"
#include "../bin/shell/shell.h"
#include "../include/gdt.h"
#include "../include/idt.h"

void kernel_main(void)
{
	vgacon_init();

	vgacon_clear(VGA_COLOR_BLACK);

	vgacon_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

	prints("Welcome!\n\n");

	prints("gdt init\n");
	gdt_setup();
	prints("idt init\n");
	idt_setup();

	shell_main();
}

void keyboard_irq_handler()
{
	prints("Keyboard IRQ");
}
