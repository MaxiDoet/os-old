#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/vga.h"

void vga_init(enum vga_mode mode)
{
	switch(mode) {
		case VGA_TEXT_MODE:
			vga_width=80;
			vga_height=25;

			vga_x, vga_y = 0;
			vga_mem = (uint16_t*) 0xB8000;
	}
}

void vga_text_put(enum vga_text_color fg, enum vga_text_color bg, size_t x, size_t y, char c)
{
	uint16_t color = fg | bg << 4;
	vga_mem[x+y*vga_width] = (uint16_t) c | color << 8;
}

/* Only avaiable in text mode */
void vga_set_color(enum vga_text_color fg, enum vga_text_color bg)
{
	vga_text_fg_color = fg;
	vga_text_bg_color = bg;
}

void vga_clear(enum vga_text_color color)
{
	/* Text mode */
	for (vga_y=0; vga_y != vga_height; vga_y++) {
		for (vga_x=0; vga_x != vga_width; vga_x++) {
			vga_text_put(color, color, vga_x, vga_y, ' ');
		}
	}

	vga_x=0;
	vga_y=0;
}

/* Later we'll add more clear method for different color palletes */
