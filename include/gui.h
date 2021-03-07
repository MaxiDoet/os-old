#ifndef _gui_h
#define _gui_h

#include "tty.h"

void gui_draw_filled_box(size_t x, size_t y, size_t width, size_t height, enum vga_color fill)
{
	// First we gotta set the cursor
	tty_setcursor(x, y);

	int x_temp=0;
	int y_temp=0;
	while(y_temp < height) {
		while(x_temp < width) {
			tty_putat(x_temp+x, y_temp+y, VGA_COLOR_WHITE, fill, ' ');
			x_temp++;
		}
		y_temp++;
		x_temp=0;
	}
}

#endif
