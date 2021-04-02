#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../bin/desktop/desktop.h"
#include "../bin/desktop/config.h"

#include "../bin/desktop/image2.h"
#include "../bin/desktop/font8x8.h"
#include "../bin/desktop/cursor.h"

void * memcpy(void* dst, const void* src, unsigned int cnt)
{
    char *pszDest = (char *)dst;
    const char *pszSource =( const char*)src;
    if((pszDest!= NULL) && (pszSource!= NULL))
    {
        while(cnt) //till cnt
        {
            //Copy byte by byte
            *(pszDest++)= *(pszSource++);
            --cnt;
        }
    }
    return dst;
}

char *malloc_ptr;
void *malloc(int size)
{
  void *ret;
  ret = (void*)malloc_ptr;
  malloc_ptr += size;
  return ret;
}

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

void drawimagebw(void *fb, multiboot_info_t *mbi, int x, int y, int w, int h, int data[], int color)
{
        int i,j;

        for (i=0; i<h; i++) {
                for (j=0; j<w; j++) {

                        if (data[i*w+j] == 1) {
                                putpixel(fb, mbi, x+j, y+i, color);
                        } else {
                                putpixel(fb, mbi, x+j, y+i, 0x0000);
                        }
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
	/*
	void *bb = (void *) malloc(mbi->framebuffer_height * mbi->framebuffer_pitch + 2 * mbi->framebuffer_width);

	drawimage(bb, mbi, 0, 0, 800, 600, image);
	drawchar(bb, mbi, 10, 10, font8x8_b, 0xfffff);

	memcpy(fb, bb, mbi->framebuffer_height * mbi->framebuffer_pitch + 2 * mbi->framebuffer_width);
	*/

	drawimage(fb, mbi, 50, 70, 610, 367, image);
	/*
        drawchar(fb, mbi, 10, 10, font8x8_A, 0xfffff);
	drawchar(fb, mbi, 20, 10, font8x8_B, 0xfffff);
	drawchar(fb, mbi, 30, 10, font8x8_C, 0xfffff);
	drawchar(fb, mbi, 40, 10, font8x8_D, 0xfffff);
	drawchar(fb, mbi, 50, 10, font8x8_E, 0xfffff);
	drawchar(fb, mbi, 60, 10, font8x8_F, 0xfffff);
	drawchar(fb, mbi, 70, 10, font8x8_G, 0xfffff);
	drawchar(fb, mbi, 80, 10, font8x8_H, 0xfffff);

	drawimagebw(fb, mbi, 500, 200, 64, 64, cursor, 0xfffff);
	*/
}

