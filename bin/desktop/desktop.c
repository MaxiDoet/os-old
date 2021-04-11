#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../bin/desktop/desktop.h"
#include "../include/kernel/kernel.h"
#include "../bin/desktop/gfx.h"
#include "../bin/desktop/font.h"
#include "../bin/desktop/image.h"
#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"

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

void copy_fb(void *dst, const void *src, size_t n)
{
    //asm volatile("rep movsq" : "+D" (dst) : "c"(n/8), "S"(src) : "cc", "memory");

	__asm__ volatile("rep movsw" : : "D" (dst), "S" (src), "c" (n / 2));
}

void desktop_init(void *fb, multiboot_info_t *mbi)
{
	void *sfb = (void *) (unsigned long) 0x0B4534B;

	mouse_add_callback(mouse_handler);

	// Test windows
	window win1 = desktop_new_window("Window 1", 300, 200, 100, 100, 0xffa200);
	window win2 = desktop_new_window("Window 2", 330, 200, 300, 200, 0x00fffb);

	windows[0] = win1;
	windows[1] = win2;

	int i=0;

	int r=255, g=0, b=0;

	int x=50;
	int y=50;
	int vx = 10;
	int vy = -5;

	while(1) {
		// Background
		draw_rectangle(sfb, mbi, 0, 0, mbi->framebuffer_width, mbi->framebuffer_height, 0xE71C);

		// Navbar
		draw_rectangle(sfb, mbi, 0, 0, mbi->framebuffer_width, 20, 0xBDD7);

		draw_line(sfb, mbi, 20, 100, 50, 300, 0x07E5);

		draw_image(sfb, mbi, 50, 10, 400, 225, image);

		if (r > 0 && b == 0) {
			r--;
			g++;
		}
		if (g > 0 && r == 0) {
			g--;
		b++;
		}
		if (b > 0 && g == 0) {
			r++;
			b++;
		}

		draw_circle_filled(sfb, mbi, 500+i, 200, 100, (((r+50 & 0xf8)<<8) + ((g+50 & 0xfc)<<3)+(b+50>>3)));
		i++;

		// Bounce
		x=x+vx;
		y=y+vy;

		if(x<0 || x>800) {
			vx=-vx;
			x=x+vx;
		}
		if(y<0 || y>600) {
			vy=-vy;
			y=y+vy;
		}

		/*
		for(int i=0; i<2; i++) {
			draw_rectangle(fb2, mbi, windows[i].x, windows[i].y, windows[i].width, windows[i].height, windows[i].background);
		}
		*/

		/*
		for(int line=0; line < mbi->framebuffer_height; line++) {
			int offset = line * mbi->framebuffer_width;

			for (int pixel=0; pixel < mbi-framebuffer_width; pixel++) {
				if ()
			}
		}
		*/

		// Cursor
		draw_image_transparent(sfb, mbi, cursorX, cursorY, 19, 27, cursor);


		copy_fb(fb, sfb, mbi->framebuffer_width * mbi->framebuffer_height * 2);
	}
}

