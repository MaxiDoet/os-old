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

void *memcpy(void *dst, const void *src, size_t n)
{
    void *ret = dst;
    asm volatile("rep movsb" : "+D" (dst) : "c"(n), "S"(src) : "cc", "memory");
    return ret;
}

void desktop_init(void *fb, multiboot_info_t *mbi)
{
	void *fb2 = (void *) (unsigned long) 0x0A4534B; // 0x034434B

	// Test windows
	window win1 = desktop_new_window("Window 1", 500, 200, 20, 30, 0xffa200);
	window win2 = desktop_new_window("Window 2", 300, 200, 400, 300, 0x00fffb);

	windows[0] = win1;
	windows[1] = win2;

	int c=0;
	int colors[] = {0xff0000, 0x37ff00, 0x00ccff, 0x0400ff, 0xffa200};

	int j=3;
        int colors2[] = {0xff0000, 0x37ff00, 0x00ccff, 0x0400ff, 0xffa200};

	while(1) {
		// Background
		drawrect(fb2, mbi, 0, 0, mbi->framebuffer_width, mbi->framebuffer_height, 0x000000);

		for(int i=0; i<2; i++) {
			drawrect(fb2, mbi, windows[i].x, windows[i].y, windows[i].width, windows[i].height, windows[i].background);
		}

		windows[0].x++;
		windows[1].x++;

		windows[0].background = colors[c];
		if (c==4) c=0;
		c++;

		windows[1].background = colors2[j];
                if (j==4) j=0;
                j++;


		memcpy(fb, fb2, mbi->framebuffer_width * mbi->framebuffer_height * 2);
	}
}

