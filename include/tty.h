#ifndef _tty_h
#define _tty_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "string.h"

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

size_t tty_fg_color;
size_t tty_bg_color;

void tty_init()
{
        vga_mem=(uint16_t*) 0xb8000; // The video memory location
        tty_column=0;
	tty_row=0;
	tty_fg_color=VGA_COLOR_WHITE; // Standard console colors
	tty_bg_color=VGA_COLOR_BLACK;
}

void tty_setcolor(enum vga_color fg, enum vga_color bg)
{
	tty_fg_color=fg;
	tty_bg_color=bg;
}

void tty_setcursor(size_t column, size_t row)
{
	tty_column=column;
	tty_row=row;
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

void tty_clear()
{
        int x=0;
        int y=0;

       	while (y != VGA_HEIGHT) {
		while(x != VGA_WIDTH) {
			tty_putat(x, y, tty_fg_color, tty_bg_color, ' ');
			x++;
		}
		x=0;
		y++;
	}
}

/*
void tty_printf(char* str)
{
	int i=0;
	while (str[i] != 0) {

		if(str[i] == '\n') {
			// Newline
			tty_row++;
			tty_column=0;

			if (str[++i] != 0) {
				continue;
			} else {
				break;
			}
		}

		tty_put(tty_fg_color, tty_bg_color, str[i]);

		i++;
	}
	/*
        int i=0;
        while (str[i] != 0) {

                if(str[i] == '\n') {
                        // Newline
                        tty_row++;
                        tty_column=0;

                        if (str[++i] != 0) {
                                continue;
                        } else {
                                break;
                        }
                }

		// Check if char is character or number
		if(strchr(printf_characters, str[i])) {
			tty_put(tty_fg_color, tty_bg_color, str[i]);
		}

		if(strchr(printf_digits, str[i])) {
			tty_put(tty_fg_color, tty_bg_color, '0' + str[i]);
		}

                i++;
        }
}
*/

#endif
