#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/vga.h"
#include "../include/print.h"
#include "../include/cmos.h"

char* itoa(int val, int base){

        static char buf[32] = {0};

        int i = 30;

        for(; val && i ; --i, val /= base)

                buf[i] = "0123456789abcdef"[val % base];

        return &buf[i+1];

}

int bin2dec(int bin)
{
	int base = 1, rem, dec=0;
	while (bin > 0) {
        	rem = bin % 10;
        	dec = dec + rem * base;
        	bin = bin / 10 ;
        	base = base * 2;
    	}

	return dec;
}


void kernel_main(void) {

	vga_init(VGA_TEXT_MODE);

	vga_clear(VGA_TEXT_COLOR_BLUE);

	vga_set_color(VGA_TEXT_COLOR_WHITE, VGA_TEXT_COLOR_BLUE);

	prints("Hello World!\nThis is the next line!\n");

	// CMOS Dump
	prints("CMOS Dump:\n");
	prints(" Hour: ");
	prints(itoa(cmos_read(4), 2));
	prints("  Min:");
	prints(itoa(cmos_read(2), 2));
	prints("  Sec:");
        prints(itoa(cmos_read(0), 2));
	prints("\n");
	prints(" Day:");
        prints(itoa(cmos_read(6), 2));
	prints("  Month:");
        prints(itoa(cmos_read(8), 2));
	prints("  Year:");
        prints(itoa(cmos_read(9), 2));
	prints("\n");
}
