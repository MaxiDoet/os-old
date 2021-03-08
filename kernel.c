#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "tty.h"
#include "rtc.h"
#include "time.h"
#include "gui.h"
#include "print.h"
#include "keyboard.h"

void print_shell_prefix()
{
	prints("> ");
}

void kernel_main(void) {
	// Init kernel time
	struct time kernel_time;

	kernel_time.sec = rtc_read(0);
	kernel_time.min = rtc_read(2);
	kernel_time.hour = rtc_read(4);
	kernel_time.day = rtc_read(7);
	kernel_time.mon = rtc_read(8)-1;
	kernel_time.year = rtc_read(9);

	tty_init();

	/*
	tty_setcolor(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	tty_clear();

	prints("Welcome!\n");
	*/

	//Fill screen with grey
	tty_setcolor(VGA_COLOR_WHITE, VGA_COLOR_DARKGRAY);
	tty_clear();

	//Draw window frame
	gui_draw_filled_box(10, 3, 60, 19, VGA_COLOR_GRAY);

	//Print window 'title'
	tty_column=37;
	tty_row=3;
	tty_setcolor(VGA_COLOR_WHITE, VGA_COLOR_GRAY);
	prints("Terminal");

	//Print icons
	tty_column=62;
	tty_row=3;
	prints("-");

	tty_column+=2;
	tty_row=3;
	prints("=");

	tty_column+=2;
	tty_row=3;
	prints("x");

	tty_column=12;
	tty_row=5;
	print_shell_prefix();

	char* input_buffer;
	char* key;

	while (1) {
		key = keyboard_input();

		if(key=="enter") {
			prints("\n");
			tty_setcolor(VGA_COLOR_DARKGRAY, VGA_COLOR_DARKGRAY);
			prints("          ");
			tty_setcolor(VGA_COLOR_WHITE, VGA_COLOR_GRAY);
			prints("  ");
			print_shell_prefix();

			if(input_buffer == "rer") {
				prints("It works!");
			}
		} else {
			prints(key);
			strcat(input_buffer, key);
		}
	}

	/*
	tty_put(VGA_COLOR_GREEN, VGA_COLOR_BLACK, '0'+kernel_time.sec);

	int i=0;
	enum vga_color colors[] = {VGA_COLOR_RED, VGA_COLOR_GREEN, VGA_COLOR_BLUE};
	// A little 'render' loop
	while (1) {
		// Draw box
		gui_draw_filled_box(5, 5, 20, 5, colors[i]);
		i++;
		if(i>2) {
			i=0;
		}
		tty_clear();
	}
	*/
}
