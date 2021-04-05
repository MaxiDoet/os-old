#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../bin/desktop/desktop.h"
#include "../bin/desktop/config.h"

#include "../bin/desktop/image2.h"
#include "../bin/desktop/font1.h"
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

void drawchar(void *fb, multiboot_info_t *mbi, int x, int y, int w, int h, int font_char[64], int color)
{
	int i,j;

	for (i=0; i<h; i++) {
		for (j=0; j<w; j++) {

			if (font_char[i*w+j] == 1) {
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

	//drawimage(fb, mbi, 50, 100, 610, 367, image);

	drawchar(fb, mbi, 50, 50, 16, 32, ad_char_H, 0xF800);
        drawchar(fb, mbi, 70, 50, 16, 32, ad_char_e, 0x07E7);
        drawchar(fb, mbi, 90, 50, 16, 32, ad_char_l, 0x001F);
        drawchar(fb, mbi, 110, 50, 16, 32, ad_char_l, 0xFFE0);
        drawchar(fb, mbi, 130, 50, 16, 32, ad_char_o, 0x07FF);
        //drawchar(fb, mbi, 100, 50, 32, char_ , 0xfffff);
        drawchar(fb, mbi, 150, 50, 16, 32, ad_char_W, 0xF800);
        drawchar(fb, mbi, 170, 50, 16, 32, ad_char_o, 0x07E7);
        drawchar(fb, mbi, 190, 50, 16, 32, ad_char_r, 0x001F);
        drawchar(fb, mbi, 210, 50, 16, 32, ad_char_l, 0xFFE0);
        drawchar(fb, mbi, 230, 50, 16, 32, ad_char_d, 0x08FF);
}

