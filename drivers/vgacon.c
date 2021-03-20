#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/vgacon.h"
#include "../include/vga-color.h"

void vgacon_init()
{
	vgacon_width=80;
	vgacon_height=25;

	vgacon_x, vgacon_y = 0;
	vgacon_mem = (uint16_t*) 0xB8000;
}

void vgacon_text_put(enum vga_color fg, enum vga_color bg, size_t x, size_t y, char c)
{
	uint16_t color = fg | bg << 4;
	vgacon_mem[x+y*vgacon_width] = (uint16_t) c | color << 8;
}

void vgacon_set_color(enum vga_color fg, enum vga_color bg)
{
	vgacon_text_fg_color = fg;
	vgacon_text_bg_color = bg;
}

void vgacon_clear(enum vga_color color)
{
	/* Text mode */
	for (vgacon_y=0; vgacon_y != vgacon_height; vgacon_y++) {
		for (vgacon_x=0; vgacon_x != vgacon_width; vgacon_x++) {
			vgacon_text_put(color, color, vgacon_x, vgacon_y, ' ');
		}
	}

	vgacon_x=0;
	vgacon_y=0;
}

/* Later we'll add more clear method for different color palletes */
