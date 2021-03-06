#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "tty.h"

void kernel_main(void) {
	tty_init();
	tty_put(VGA_COLOR_WHITE, VGA_COLOR_BLACK, 'H');
	tty_put(VGA_COLOR_WHITE, VGA_COLOR_BLACK, 'e');
	tty_put(VGA_COLOR_WHITE, VGA_COLOR_BLACK, 'l');
	tty_put(VGA_COLOR_WHITE, VGA_COLOR_BLACK, 'l');
	tty_put(VGA_COLOR_WHITE, VGA_COLOR_BLACK, 'o');
}
