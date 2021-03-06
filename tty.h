#ifndef _tty_h
#define _tty_h
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint16_t* vga_mem;
size_t tty_column;
size_t tty_row;

const size_t VGA_WIDTH = 80;
const size_t VGA_HEIGHT = 25;

enum vga_color {
        VGA_COLOR_BLACK = 0,
        VGA_COLOR_BLUE = 1,
        VGA_COLOR_GREEN = 2,
        VGA_COLOR_CYAN = 3,
        VGA_COLOR_RED = 4,
        VGA_COLOR_PURPLE = 5,
        VGA_COLOR_BROWN = 6,
        VGA_COLOR_GRAY = 7,
        VGA_COLOR_DARKGRAY = 8,
        VGA_COLOR_LIGHTBLUE = 9,
        VGA_COLOR_LIGHTGREEN = 10,
        VGA_COLOR_LIGHTCYAN = 11,
        VGA_COLOR_LIGHTRED = 12,
        VGA_COLOR_LIGHTPURPLE = 13,
        VGA_COLOR_YELLOW = 14,
        VGA_COLOR_WHITE = 15
};

void tty_init()
{
        vga_mem=(uint16_t*) 0xb8000;
        tty_column=0;
	tty_row=0;
}

void tty_putat(size_t x, size_t y, enum vga_color fg, enum vga_color bg, char c)
{
        uint16_t color = fg | bg << 4;
        vga_mem[x+y*VGA_WIDTH] = (uint16_t) c | color << 8;
}

void tty_put(enum vga_color fg, enum vga_color bg, char c)
{
	tty_putat(tty_column, tty_row, fg, bg, c);
	if(++tty_column == VGA_WIDTH) {
		tty_column=0;
		if(++tty_row == VGA_HEIGHT) {
			tty_row=0;
		}
	}
}
