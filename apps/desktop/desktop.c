#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../apps/desktop/desktop.h"
#include "../include/kernel/kernel.h"
#include "../libc/include/mm.h"
#include "../libc/include/string.h"

#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"
#include "../include/drivers/serial.h"

#include "../config.h"

#include "../apps/desktop/font.h"
#include "../apps/desktop/wallpaper.h"
#include "../apps/desktop/cursor.h"

#include "../lib/gui/context.h"
#include "../lib/gui/direct.h"
#include "../lib/gui/window.h"

window windows[10];

int cursorX=50;
int cursorY=50;
context main_context;

static void mouse_handler(struct mouse_event event)
{
	cursorX = event.x * 10;
	cursorY = event.y * 10;

	for (int i=0; i < 10; i++) {
		if (!windows[i].grabbed && event.button1_pressed && cursorX >= windows[i].x-10 && cursorX <= windows[i].x-10 + windows[i].width && cursorY >= windows[i].y-10 && cursorY <= windows[i].y-10 + windows[i].height) {
			kdebug("Grab");
			windows[i].grabbed = true;
		}

		if (windows[i].grabbed && !event.button1_pressed) {
			windows[i].grabbed = false;
		}
	}
}

void keyboard_handler(struct keyboard_event event)
{

}

void desktop_swap_fb()
{
	__asm__ volatile("rep movsw" : : "D" (bb), "S" (fb), "c" ((main_context.width * main_context.height * 2) / 2));
}

void desktop_init(unsigned long fbaddr, int width, int height, int pitch)
{
	bb = (void *) (unsigned long) fbaddr;
	fb = (void *) (unsigned long) 0x0B4534B;

	main_context.x = 0;
	main_context.y = 0;
	main_context.width = width;
	main_context.height = height;
	main_context.fb_pitch = pitch;
	main_context.fb = fb;

	window test;
	test.x = 70;
	test.y = 70;
	test.width = 100;
	test.height = 50;

	windows[0] = test;

	keyboard_add_callback(keyboard_handler);
	mouse_add_callback(mouse_handler);

	while(1) {
		// Background
		#ifdef DESKTOP_WALLPAPER
			draw_image(main_context, 0, 0, main_context.width, main_context.height, wallpaper_image);
		#else
			draw_filled_rectangle(main_context, 0, 0, main_context.width, main_context.height, 0x4A69);
		#endif

		for (int i=0; i < 10; i++) {
			draw_window(main_context, windows[i]);

			if (windows[i].grabbed) {
				windows[i].x = cursorX;
				windows[i].y = cursorY;
			}
		}

		// Cursor
		draw_image_transparent(main_context, cursorX, cursorY, 19, 27, cursor);

		// Swap frontbuffer and backbuffer
                desktop_swap_fb();
	}
}

