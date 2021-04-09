#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../bin/desktop/desktop.h"
#include "../bin/desktop/gfx.h"

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

void memcpy(void *dst, const void *src, size_t n)
{
    asm volatile("rep movsb" : "+D" (dst) : "c"(n), "S"(src) : "cc", "memory");
}

void desktop_init(void *fb, multiboot_info_t *mbi)
{
	void *fb2 = (void *) (unsigned long) 0x0A4534B; // 0x034434B

	// Test windows
	window win1 = desktop_new_window("Window 1", 300, 200, 100, 100, 0xffa200);
	window win2 = desktop_new_window("Window 2", 330, 200, 300, 200, 0x00fffb);

	windows[0] = win1;
	windows[1] = win2;

	int i=0;

	while(1) {
		// Background
		draw_rectangle(fb2, mbi, 0, 0, mbi->framebuffer_width, mbi->framebuffer_height, 0xE71C);

		// Navbar
		draw_rectangle(fb2, mbi, 0, 0, mbi->framebuffer_width, 20, 0xBDD7);

		draw_line(fb2, mbi, 20, 100, 50, 300, 0x07E5);


		draw_circle_filled(fb2, mbi, 500+i, 200, 100, 0x07E5);
		i++;
		/*
		for(int i=0; i<2; i++) {
			draw_rectangle(fb2, mbi, windows[i].x, windows[i].y, windows[i].width, windows[i].height, windows[i].background);
		}
		*/

		memcpy(fb, fb2, mbi->framebuffer_width * mbi->framebuffer_height * 2);
	}
}

