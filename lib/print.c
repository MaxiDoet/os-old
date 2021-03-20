#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/print.h"
#include "../include/vgacon.h"

void printc(char c)
{
	switch(c) {
		case '\n':
                	vgacon_x=0;
                        vgacon_y++;
                        break;
		case '\0':
			break;
               default:
			vgacon_text_put(vgacon_text_fg_color, vgacon_text_bg_color, vgacon_x, vgacon_y, c);
                        vgacon_x++;
	}

        if (vgacon_x >= vgacon_width) {
       		vgacon_x=0;
		vgacon_y++;
	}

	if (vgacon_y >= vgacon_height) {
		vgacon_clear(vgacon_text_bg_color);
		vgacon_x=0;
		vgacon_y=0;
	}
}

void erase(int num) {
	for (int i=0; i<num; i++) {
		if (vgacon_x == 0 && vgacon_y == 0) {
			return;
		} else if (vgacon_x == 0) {
			vgacon_x = vgacon_width - 1;
			vgacon_y--;
		} else {
			vgacon_x--;
		}

		vgacon_text_put(vgacon_text_fg_color, vgacon_text_bg_color, vgacon_x, vgacon_y, ' ');
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

