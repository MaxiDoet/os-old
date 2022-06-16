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

#include <gui/context.h>
#include <gui/window.h>
#include <gui/direct.h>

static window *windows[10];
static int cursor_x;
static int cursor_y;
context main_context;

static void *fb;
static void *bb;

void draw_window(context ctx, window *win)
{
	// Frame
	draw_filled_rectangle(ctx, win->x, win->y, win->width, 30, FRAME_COLOR);

	// Window title
	draw_string(ctx, win->x + 10, win->y, win->title, 0xFFFF);

	// Content
	draw_filled_rectangle(ctx, win->x, win->y + 20, win->width, win->height - 20, WINDOW_COLOR);

	// Frame buttons
	draw_circle_filled(ctx, win->x + win->width - 40, win->y + 8, 5, 0x5ECC);
	draw_circle_filled(ctx, win->x + win->width - 25, win->y + 8, 5, 0xFE26);
	draw_circle_filled(ctx, win->x + win->width - 10, win->y + 8, 5, 0xF28A);
}


static void mouse_handler(struct mouse_event event)
{
	int movement_x = event.x * 2;
	int movement_y = event.y * 2;

	// Update cursor position
	if (!main_context.grabbing) {
		cursor_x += movement_x;
		cursor_y -= movement_y;
	}

	if (main_context.grabbing && main_context.grabbed_window) {
		main_context.grabbed_window->x += movement_x;
		main_context.grabbed_window->y -= movement_y;
	}

	if (main_context.grabbing && main_context.grabbed_window && !main_context.grabbed_window->coliding) {
		cursor_x += movement_x;
		cursor_y -= movement_y;
	}

	// Check for collision
	if (cursor_x < 0) cursor_x = 0;
	if (cursor_x > main_context.width) cursor_x = main_context.width;
	if (cursor_y < 0) cursor_y = 0;
	if (cursor_y > main_context.height) cursor_y = main_context.height;

	for (int i=0; i < 1; i++) {
		if (windows[i] == NULL) continue;

		/* Window grabbing */
		if (!windows[i]->grabbed && event.button1_pressed && cursor_x >= windows[i]->x-10 && cursor_x <= windows[i]->x-10 + windows[i]->width && cursor_y >= windows[i]->y-10 && cursor_y <= windows[i]->y-10 + 20) {
			windows[i]->grabbed = true;
			windows[i]->focused = true;

			main_context.grabbing = true;
			main_context.grabbed_window = windows[i];
		}

		if (windows[i]->grabbed && !event.button1_pressed) {
			windows[i]->grabbed = false;

			main_context.grabbing = false;
			main_context.grabbed_window = NULL;
		}

		/* Window collision */
		if (windows[i]->x + windows[i]->width >= main_context.width) {
			windows[i]->x = main_context.width - windows[i]->width;
		}

		if (windows[i]->y + windows[i]->height >= main_context.height) {
			windows[i]->y = main_context.height - windows[i]->height;
		}

		if (windows[i]->x <= 0) {
			windows[i]->x = 0;
		}

		if (windows[i]->y <= 0) {
			windows[i]->y = 0;
		}

		if (windows[i]->x + windows[i]->width >= main_context.width ||
			windows[i]->y + windows[i]->height >= main_context.height ||
			windows[i]->x <= 0 ||
			windows[i]->y <= 0) 
		{
			windows[i]->coliding = true;
		} else {
			windows[i]->coliding = false;
		}
	}
}

void keyboard_handler(struct keyboard_event event)
{

}

static inline void desktop_swap_fb()
{
	__asm__ volatile("rep movsb" : : "D" (bb), "S" (fb), "c" ((main_context.width * main_context.height * 4) / 2));
}

void desktop_init(unsigned long fbaddr, int width, int height, int pitch)
{
	bb = (void *) (unsigned long) fbaddr;
	fb = (void *) (unsigned long) malloc(800 * 600);

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

	windows[0] = &test;

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
			if (windows[i] == NULL) continue;
			draw_window(main_context, windows[i]);
		}

		// Cursor
		draw_image_transparent(main_context, cursor_x, cursor_y, 13, 24, cursor);

		// Swap frontbuffer and backbuffer
    	desktop_swap_fb();
	}
}

