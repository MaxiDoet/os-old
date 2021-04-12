#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../bin/desktop/desktop.h"
#include "../include/kernel/kernel.h"
#include "../bin/desktop/ui.h"
#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"

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

void mouse_handler(struct mouse_event event)
{
	cursorX = event.x * 10;
	cursorY = event.y * 10;

	kdebug("CursorX: %d, CursorY: %d\r\n", cursorX, cursorY);
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

	mouse_add_callback(mouse_handler);

	while(1) {
		// Background
		#ifdef DESKTOP_WALLPAPER
			draw_image(0, 0, fb_width, fb_height, wallpaper_image);
		#else
			draw_rectangle(0, 0, fb_width, fb_height, 0x4A69);
		#endif

		// Navbar
		draw_rectangle(0, 0, fb_width, 20, 0x4228);

		// Cursor
		draw_image_transparent(cursorX, cursorY, 19, 27, cursor);

		// Swap frontbuffer and backbuffer
		desktop_swap_fb();
	}
}

