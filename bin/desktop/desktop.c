#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../bin/desktop/desktop.h"
#include "../include/kernel/kernel.h"
#include "../bin/desktop/ui.h"

#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"
#include "../include/drivers/serial.h"

#include "../config.h"

#include "../bin/desktop/font.h"
#include "../bin/desktop/wallpaper.h"
#include "../bin/desktop/cursor.h"

window windows[10];

window desktop_new_window(char* title, int width, int height, int x, int y, int background)
{
	window win;

	win.title = title;
	win.width = width;
	win.height = height;
	win.x = x;
	win.y = y;
	win.background = background;

	return win;
}

int cursorX=0;
int cursorY=0;

char* str="";
int i=0;

void mouse_handler(struct mouse_event event)
{
	cursorX = event.x * 10;
	cursorY = event.y * 10;
}

void keyboard_handler(struct keyboard_event event)
{
	if (!event.released) {
		str[i]=event.asci;
		i++;
	}
}

void desktop_swap_fb()
{
	__asm__ volatile("rep movsw" : : "D" (bb), "S" (fb), "c" ((fb_width * fb_height * 2) / 2));
}

void desktop_init(unsigned long fbaddr, int width, int height, int pitch)
{
	bb = (void *) (unsigned long) fbaddr;
	fb = (void *) (unsigned long) 0x0B4534B;

	fb_width = width;
	fb_height = height;
	fb_pitch = pitch;

	int progress=0;

	while(progress<100) {
		draw_filled_rectangle(0, 0, fb_width, fb_height, 0x2104);
		draw_filled_rectangle((fb_width / 2) - 100, 400, progress, 10, 0xFFFF);
		progress+=2;

		// Swap frontbuffer and backbuffer
		desktop_swap_fb();
	}

	keyboard_add_callback(keyboard_handler);
	mouse_add_callback(mouse_handler);

	while(1) {
		// Background
		#ifdef DESKTOP_WALLPAPER
			draw_image(0, 0, fb_width, fb_height, wallpaper_image);
		#else
			draw_filled_rectangle(0, 0, fb_width, fb_height, 0x4A69);
		#endif

		// Navbar
		draw_filled_rectangle(0, 0, fb_width, 20, 0x4228);

		// Cursor
		draw_image_transparent(cursorX, cursorY, 19, 27, cursor);

		draw_rounded_rectangle(70, 70, 40, 40, 0x4228);

		draw_string(50, 50, font, str, 0x4228);

		// Swap frontbuffer and backbuffer
                desktop_swap_fb();
	}
}

