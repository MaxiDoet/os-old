#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../bin/desktop/desktop.h"
#include "../bin/desktop/config.h"

#include "../bin/desktop/image.h"
#include "../bin/desktop/font8x8.h"

void putpixel(void *fb, multiboot_info_t *mbi, int x, int y, int color)
{
	multiboot_uint16_t *pixel = fb + mbi->framebuffer_pitch * y + 2 * x;
        *pixel = color;
}

void drawrect(void *fb, multiboot_info_t *mbi, int x, int y, int w, int h, int color)
{
	int i,j;

	for (i=0; i < h; i++) {
		for (j=0; j < w; j++) {
			putpixel(fb, mbi, x+j, y+i, color);
		}
	}
}

void drawimage(void *fb, multiboot_info_t *mbi, int x, int y, int w, int h, int data[])
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

void drawchar(void *fb, multiboot_info_t *mbi, int x, int y, int font_char[64], int color)
{
	int i,j;

	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {

			if (font_char[i*8+j] == 1) {
				putpixel(fb, mbi, x+j, y+i, color);
			} else {
				putpixel(fb, mbi, x+j, y+i, 0x0000);
			}
		}
	}
}
void desktop_init(void *fb, multiboot_info_t *mbi)
{
	// Background
	//drawrect(fb, mbi, 0, 0, mbi->framebuffer_width, mbi->framebuffer_height, DESKTOP_BACKGROUND);

	//drawchar(fb, mbi, 10, 10, font8x8_b, 0xfffff);
	drawimage(fb, mbi, 0, 0, 610, 367, image);
}
