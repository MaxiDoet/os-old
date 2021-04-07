#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../bin/desktop/desktop.h"
#include "../bin/desktop/gfx.h"

window windows[10];

window desktop_new_window(char* title, int width, int height, int x, int y)
{
	window win;

	win.title = title;
	win.width = width;
	win.height = height;
	win.x = x;
	win.y = y;

	return win;
}

void memcpy(void *dest, void *src, size_t n)
{
   // Typecast src and dest addresses to (char *)
   char *csrc = (char *)src;
   char *cdest = (char *)dest;
  
   // Copy contents of src[] to dest[]
   for (int i=0; i<n; i++)
       cdest[i] = csrc[i];
}

void desktop_init(void *fb, multiboot_info_t *mbi)
{
	void *fb2 = (void *) (unsigned long) 0x034434B;

	// Test windows
	window win1 = desktop_new_window("Window 1", 500, 200, 20, 30);
	window win2 = desktop_new_window("Window 2", 300, 200, 400, 300);

	windows[0] = win1;
	windows[1] = win2;

	while(1) {
		// Background
		drawrect(fb2, mbi, 0, 0, mbi->framebuffer_width, mbi->framebuffer_height, 0xCE79);

		for(int i=0; i<2; i++) {
			drawrect(fb2, mbi, windows[i].x, windows[i].y, windows[i].width, windows[i].height, 0x632C);
		}

		windows[0].x--;

		memcpy(fb, fb2, 10000000);
	}
}

