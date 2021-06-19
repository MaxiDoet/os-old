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

#include "../apps/desktop/wallpaper.h"
#include "../apps/desktop/cursor.h"
#include "../apps/desktop/logo.h"

#include "../lib/gui/context.h"
#include "../lib/gui/direct.h"
#include "../lib/gui/window.h"

window windows[10];
static int cursor_x;
static int cursor_y;
context main_context;

static void mouse_handler(struct mouse_event event)
{
	cursor_x += event.x * 2;
	cursor_y -= event.y * 2;

	// Collision
	if (cursor_x <= 0) cursor_x = 0;
	if (cursor_x >= main_context.width - 8) cursor_x = main_context.width - 8;
	if (cursor_y <= 0) cursor_y = 0;
	if (cursor_y >= main_context.height - 1) cursor_y = main_context.height - 1;

	for (int i=0; i < 10; i++) {
		if (!windows[i].grabbed && event.button1_pressed && cursor_x >= windows[i].x-10 && cursor_x <= windows[i].x-10 + windows[i].width && cursor_y >= windows[i].y-10 && cursor_y <= windows[i].y-10 + 20) {
			windows[i].grabbed = true;
			windows[i].focused = true;
		}

		if (windows[i].grabbed && !event.button1_pressed) {
			windows[i].grabbed = false;
		}
	}
}

void keyboard_handler(struct keyboard_event event)
{

}

static void desktop_swap_fb()
{
	__asm__ volatile("rep movsb" : : "D" (bb), "S" (fb), "c" ((main_context.width * main_context.height * 4) / 2));
}

/*
void desktop_draw_cursor(int data[])
{
	for (int i=0; i < 19; i++) {
		for (int j=0; j < 12; j++) {
			if (data[j*i] == 0) {
				draw_pixel(main_context, j + cursor_x, i + cursor_y, 0);
			} else if (data[j*i] == 1) {
				draw_pixel(main_context, j + cursor_x, i + cursor_y, 0xFFFFF);
			} else {
				continue;
			}
		}
	}
}
*/

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

	cursor_x = main_context.width / 2;
	cursor_y = main_context.height / 2;

	window test;
	test.x = 70;
	test.y = 70;
	test.width = 400;
	test.height = 300;

	windows[0] = test;

	keyboard_add_callback(keyboard_handler);
	mouse_add_callback(mouse_handler);

	for (int i=0; i < 1000; i++) {
		draw_filled_rectangle(main_context, 0, 0, main_context.width, main_context.height, 0x000);

		draw_string(main_context, width / 2 - (10 * strlen("Welcome")), height / 2, "Welcome", 0xffff);

		desktop_swap_fb();

	}

	while(1) {
		// Background
		#ifdef DESKTOP_WALLPAPER
			draw_image(main_context, 0, 0, main_context.width, main_context.height, wallpaper_image);
		#else
			draw_filled_rectangle(main_context, 0, 0, main_context.width, main_context.height, 0x4A69);
		#endif

		for (int i=0; i < 10; i++) {
			if (windows[i].width == 0 || windows[i].height == 0) continue;

			draw_window(main_context, windows[i]);

			if (windows[i].grabbed) {
				windows[i].x = cursor_x;
				windows[i].y = cursor_y;
			}

			if (windows[i].x + windows[i].width >= main_context.width) {
				windows[i].x = main_context.width - windows[i].width;
			}

			if (windows[i].y + windows[i].height >= main_context.height) {
				windows[i].y = main_context.height - windows[i].height;
			}
		}

		// Cursor
		draw_image_transparent(main_context, cursor_x, cursor_y, 13, 24, cursor);

		// Swap frontbuffer and backbuffer
                desktop_swap_fb();
	}
}

