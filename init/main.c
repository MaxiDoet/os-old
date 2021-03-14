#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/vga.h"
#include "../include/print.h"

void kernel_main(void) {

	vga_init(VGA_TEXT_MODE);

	vga_clear(VGA_TEXT_COLOR_BLUE);

	vga_set_color(VGA_TEXT_COLOR_WHITE, VGA_TEXT_COLOR_BLUE);

	printf("Hello World!\nThis is the next line!");
}
