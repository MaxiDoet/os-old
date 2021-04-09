#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../bin/desktop/desktop.h"
#include "../include/kernel/kernel.h"
#include "../bin/desktop/gfx.h"
#include "../bin/desktop/font.h"

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

/*
void memcpy(void *dest, void *src, size_t n)
{
   // Typecast src and dest addresses to (char *)
   char *csrc = (char *)src;
   char *cdest = (char *)dest;

   // Copy contents of src[] to dest[]
   for (int i=0; i<n; i++)
       cdest[i] = csrc[i];
}
*/

/*
void memcpy(void *dst, const void *src, size_t n)
{
    asm volatile("rep movsb" : "+D" (dst) : "c"(n), "S"(src) : "cc", "memory");
}
*/

void memcpy(void *dst, const void *src, size_t n)
{
    //asm volatile("rep movsq" : "+D" (dst) : "c"(n/8), "S"(src) : "cc", "memory");

	__asm__ volatile("rep movsw" : : "D" (dst), "S" (src), "c" (n / 2));
}

void desktop_init(void *fb, multiboot_info_t *mbi)
{
	void *sfb = (void *) (unsigned long) 0x0B4534B;

	// Test windows
	window win1 = desktop_new_window("Window 1", 300, 200, 100, 100, 0xffa200);
	window win2 = desktop_new_window("Window 2", 330, 200, 300, 200, 0x00fffb);

	windows[0] = win1;
	windows[1] = win2;

	int i=0;

	int r=255, g=0, b=0;

	//int font[][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1}};

	for(int j=0; j<64; j++) {
		kdebug("Data: %d\r\n", font[0][j]);
	}

	while(1) {
		// Background
		draw_rectangle(sfb, mbi, 0, 0, mbi->framebuffer_width, mbi->framebuffer_height, 0xE71C);

		// Navbar
		draw_rectangle(sfb, mbi, 0, 0, mbi->framebuffer_width, 20, 0xBDD7);

		draw_line(sfb, mbi, 20, 100, 50, 300, 0x07E5);

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

		draw_circle_filled(sfb, mbi, 500+i, 200, 100, (((r & 0xf8)<<8) + ((g & 0xfc)<<3)+(b>>3)));
		i++;

		draw_char(sfb, mbi, 40, 50, font[87], 'W', 0x02DF);
		draw_char(sfb, mbi, 60, 50, font[101], 'e', 0x02DF);
 		draw_char(sfb, mbi, 80, 50, font[108], 'l', 0x02DF);
		draw_char(sfb, mbi, 100, 50, font[99], 'c', 0x02DF);
                draw_char(sfb, mbi, 120, 50, font[111], 'o', 0x02DF);
                draw_char(sfb, mbi, 140, 50, font[109], 'm', 0x02DF);
		draw_char(sfb, mbi, 160, 50, font[101], 'e', 0x02DF);

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

		memcpy(fb, sfb, mbi->framebuffer_width * mbi->framebuffer_height * 2);
	}
}

