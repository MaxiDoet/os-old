#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/print.h"
#include "../include/vga.h"

void puts(char c) {
	vga_text_put(vga_text_fg_color, vga_text_bg_color, vga_x, vga_y, c);

	if (++vga_x == vga_width) {
		vga_x=0;
		if (++vga_y == vga_height) {
			vga_y=0;
		}
	}
}

void printf(char* str)
{
	int i=0;
	while (str[i]!=0) {

		if (str[i]=='\n') {
			vga_x=0;
			vga_y++;

			if (str[++i] != 0) {
				continue;
			} else {
				break;
			}
		}

		puts(str[i]);

		i++;
	}
}
