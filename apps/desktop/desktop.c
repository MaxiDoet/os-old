#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../apps/desktop/desktop.h"
#include "../include/kernel/kernel.h"
#include "../libc/include/mm.h"
#include "../libc/include/string.h"
#include "../apps/desktop/ui.h"

#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"
#include "../include/drivers/serial.h"

#include "../config.h"

#include "../apps/desktop/font.h"
#include "../apps/desktop/wallpaper.h"
#include "../apps/desktop/cursor.h"

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

int cursorX=50;
int cursorY=50;

char* str;
int i=0;

// Grabbing test
int windowX = 70;
int windowY = 70;
bool grabbing;

void mouse_handler(struct mouse_event event)
{
	cursorX = event.x * 10;
	cursorY = event.y * 10;

	if (!grabbing && event.button1_pressed && cursorX >= windowX-10 && cursorX <= windowX-10 + 200 && cursorY >= windowY-10 && cursorY <= windowY-10 + 100) {
		grabbing = true;
	}

	if (grabbing && !event.button1_pressed) {
		grabbing = false;
	}
}

void keyboard_handler(struct keyboard_event event)
{
	if (event.pressed) {
		if (i >= 50) {
			i=0;
			memset(str, 0, 100);
			return;
		}


		if (event.printable) {
			str[i]=event.asci;
			i++;
		}
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
	//fb = (void *) malloc(mm, fb_width * fb_height * 10);

	fb_width = width;
	fb_height = height;
	fb_pitch = pitch;

	int progress=0;

	while(progress<100) {
		draw_rounded_rectangle(0, 0, fb_width, fb_height, 2, 0x2104);
		draw_rounded_rectangle((fb_width / 2) - 100, 400, progress, 10, 2, 0xFFFF);
		progress+=1;

		// Swap frontbuffer and backbuffer
		desktop_swap_fb();
	}

	str = (char *) malloc(mm, 100);

	keyboard_add_callback(keyboard_handler);
	mouse_add_callback(mouse_handler);

	while(1) {
		// Background
		#ifdef DESKTOP_WALLPAPER
			draw_image(0, 0, fb_width, fb_height, wallpaper_image);
		#else
			draw_filled_rectangle(0, 0, fb_width, fb_height, 0x4A69);
		#endif

		draw_rounded_rectangle(windowX, windowY, 200, 100, 5, 0x4228);

		draw_circle_filled(windowX+160, windowY + 7, 5, 0x5ECC);
		draw_circle_filled(windowX+175, windowY + 7, 5, 0xFE26);
		draw_circle_filled(windowX+190, windowY + 7, 5, 0xF28A);

		if (grabbing) {
			windowX = cursorX;
			windowY = cursorY;
		}

		draw_string(50, 30, font, str, 0x4228);

		// Navbar
		draw_filled_rectangle(0, 0, fb_width, 20, 0x4228);

		// Cursor
		draw_image_transparent(cursorX, cursorY, 19, 27, cursor);

		// Swap frontbuffer and backbuffer
                desktop_swap_fb();
	}
}

