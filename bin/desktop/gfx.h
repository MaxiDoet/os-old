#include "../include/kernel/kernel.h"

void putpixel(void *fb, multiboot_info_t *mbi, int x, int y, int color)
{
        multiboot_uint16_t *pixel = fb + mbi->framebuffer_pitch * y + 2 * x;
        *pixel = color;
}

void draw_horizontal_line(void *fb, multiboot_info_t *mbi, int x, int y, int w, int color)
{
	int i;

	multiboot_uint16_t *pixel = fb + mbi->framebuffer_pitch * y + 2 * x;

	for (i=0; i<w; i++) {
		*pixel = color;
		*pixel++;
	}
}

void draw_rectangle(void *fb, multiboot_info_t *mbi, int x, int y, int w, int h, int color)
{
	int i;

	for(i=0; i<h; i++) {
		draw_horizontal_line(fb, mbi, x, y+i, w, color);
	}
}

void draw_line(void *fb, multiboot_info_t *mbi, int x0, int y0, int x1, int y1, int color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int D = 2*dy - dx;
	int y = y0;
	int x;

	for (x=x0; x<x1; x++) {
		putpixel(fb, mbi, x, y, color);
		if (D > 0) {
			y++;
			D-=2*dx;
		}
		D+=2*dy;
	}
}

void draw_circle(void *fb, multiboot_info_t *mbi, int x0, int y0, int radius, int color)
{
	/* Source: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm */

	int f = 1-radius;
	int ddF_x = 0;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	putpixel(fb, mbi, x0, y0 + radius, color);
	putpixel(fb, mbi, x0, y0 - radius, color);
	putpixel(fb, mbi, x0 + radius, y0, color);
	putpixel(fb, mbi, x0 - radius, y0, color);

	while(x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x + 1;

		putpixel(fb, mbi, x0 + x, y0 + y, color);
		putpixel(fb, mbi, x0 - x, y0 + y, color);
		putpixel(fb, mbi, x0 + x, y0 - y, color);
		putpixel(fb, mbi, x0 - x, y0 - y, color);
		putpixel(fb, mbi, x0 + y, y0 + x, color);
		putpixel(fb, mbi, x0 - y, y0 + x, color);
		putpixel(fb, mbi, x0 + y, y0 - x, color);
		putpixel(fb, mbi, x0 - y, y0 - x, color);
	}
}

void draw_circle_filled(void *fb, multiboot_info_t *mbi, int x0, int y0, int radius, int color)
{
	/* Source: https://stackoverflow.com/a/14976268/13378037 */

	int x = radius;
	int y = 0;
	int xChange = 1 - (radius << 1);
	int yChange = 0;
	int radiusError = 0;

	while (x >= y) {
		for(int i = x0 - x; i <= x0 + x; i++) {
			putpixel(fb, mbi, i, y0 + y, color);
			putpixel(fb, mbi, i, y0 - y, color);
		}
		for(int i = x0 - y; i <= x0 + y; i++) {
			putpixel(fb, mbi, i, y0 + x, color);
			putpixel(fb, mbi, i, y0 - x, color);
		}

		y++;
		radiusError += yChange;
		yChange += 2;
		if(((radiusError << 1) + xChange) > 0) {
			x--;
			radiusError += xChange;
			xChange += 2;
		}
	}
}

void draw_monochrome_bitmap(void *fb, multiboot_info_t *mbi, int x, int y, int *bitmap, int color)
{
	for(int i=0; i < 32; i++) {
		for(int j=0; j < 32; j++) {
			if(bitmap[j+i*32] == 1) {
				putpixel(fb, mbi, x+j, y+i, color);
			} else {
				continue;
			}
		}
	}
}

void draw_string(void *fb, multiboot_info_t *mbi, int x, int y, int font[][1024], char* str, int color)
{
	int j=0;
	for(int i=0; str[i] != '\0'; i++) {
		draw_monochrome_bitmap(fb, mbi, x+j, y, font[str[i]], color);
		j+=10;
	}
}

void draw_image(void *fb, multiboot_info_t *mbi, int x, int y, int w, int h, int data[])
{
        int i,j;

        for (i=0; i<h; i++) {
                for (j=0; j<w; j++) {
                        int color24 = data[i*w+j];
                        int color16;
                        color16 = (((color24&0xf80000)>>8) + ((color24&0xfc00)>>5) + ((color24&0xf8)>>3));
                        putpixel(fb, mbi, x+j, y+i, color16);
                }
        }
}
