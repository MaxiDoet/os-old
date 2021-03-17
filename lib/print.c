#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/print.h"
#include "../include/vga.h"

void printc(char c)
{
	switch(c) {
		case '\n':
                	vga_x=0;
                        vga_y++;
                        break;
               default:
			vga_text_put(vga_text_fg_color, vga_text_bg_color, vga_x, vga_y, c);
                        vga_x++;
	}

        if (vga_x >= vga_width) {
       		vga_x=0;
		vga_y++;
	}

	if (vga_y >= vga_height) {
		vga_clear(vga_text_bg_color);
		vga_x=0;
		vga_y=0;
	}
}

void prints(char* str)
{
	int i=0;
	while (str[i]!=0) {
		printc(str[i]);
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

void printh8(uint8_t key)
{
    	char* out = "00";
    	char* hex = "0123456789ABCDEF";
    	out[0] = hex[(key >> 4) & 0xF];
    	out[1] = hex[key & 0xF];
    	prints(out);
}

void printh16(uint16_t key)
{
	printh8((key >> 8) & 0xFF);
   	printh8( key & 0xFF);
}

