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

void prints(char* str)
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

void printi(int val)
{
	int base=10;
        static char buf[32] = {0};

        int i = 30;

        for(; val && i ; --i, val /= base)

                buf[i] = "0123456789abcdef"[val % base];

        prints (&buf[i+1]);
}

void printd(int val, int base)
{
        static char buf[32] = {0};

        int i = 30;

        for(; val && i ; --i, val /= base)

                buf[i] = "0123456789abcdef"[val % base];

        prints(&buf[i+1]);
}

void printb(uint8_t b)
{
	printd(b, 2);
}
