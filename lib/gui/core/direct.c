#include <stdint.h>

#include <gui/direct.h>
#include <gui/font.h>

void draw_pixel(context ctx, int x, int y, int color)
{
	x += ctx.x;
	y += ctx.y;

        uint16_t *pixel = ctx.fb + ctx.fb_pitch * y + 2 * x;
        *pixel = color;
}

void draw_horizontal_line(context ctx, int x, int y, int w, int color)
{
	x += ctx.x;
	y += ctx.y;
        int i;

        uint16_t *pixel = ctx.fb + ctx.fb_pitch * y + 2 * x;

        for (i=0; i<w; i++) {
                *pixel = color;
                *pixel++;
        }
}

void draw_vertical_line(context ctx, int x, int y, int h, int color)
{
	x += ctx.x;
	y += ctx.y;
        int i;

        uint16_t *pixel = ctx.fb + ctx.fb_pitch * y + 2 * x;

        for (i=0; i<h; i++) {
                *pixel = color;
                pixel = ctx.fb + ctx.fb_pitch * (y+i) + 2 * x;
        }
}

void draw_line(context ctx, int x0, int y0, int x1, int y1, int color)
{
        int dx = x1 - x0;
        int dy = y1 - y0;
        int D = 2*dy - dx;
        int y = y0;
        int x;

        for (x=x0; x<x1; x++) {
                draw_pixel(ctx, x, y, color);
                if (D > 0) {
                        y++;
                        D-=2*dx;
                }
                D+=2*dy;
        }
}

void draw_filled_rectangle(context ctx, int x, int y, int w, int h, int color)
{
        int i;

        for(i=0; i<h; i++) {
                draw_horizontal_line(ctx, x, y+i, w, color);
        }
}

void draw_rectangle(context ctx, int x, int y, int w, int h, int size, int color)
{
        if (size==1) {
                // Top, Bottom
                draw_horizontal_line(ctx, x, y, w, color);
                draw_horizontal_line(ctx, x, y+h, w, color);
                // Left, Right
                draw_vertical_line(ctx, x, y, h, color);
                draw_vertical_line(ctx, x+w, y, h, color);
        } else {
                // Top, Bottom
                draw_filled_rectangle(ctx, x, y, w, size, color);
                draw_filled_rectangle(ctx, x, y+h, w, size, color);
                // Left, Right
                draw_filled_rectangle(ctx, x, y, size, h, color);
                draw_filled_rectangle(ctx, x+w, y, size, h+size, color);
        }
}

void draw_circle(context ctx, int x0, int y0, int radius, int color)
{
        /* Source: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm */

        int f = 1-radius;
        int ddF_x = 0;
        int ddF_y = -2 * radius;
        int x = 0;
        int y = radius;

        draw_pixel(ctx, x0, y0 + radius, color);
        draw_pixel(ctx, x0, y0 - radius, color);
        draw_pixel(ctx, x0 + radius, y0, color);
        draw_pixel(ctx, x0 - radius, y0, color);

        while(x < y) {
                if (f >= 0) {
                        y--;
                        ddF_y += 2;
                        f += ddF_y;
                }
                x++;
                ddF_x += 2;
                f += ddF_x + 1;

                draw_pixel(ctx, x0 + x, y0 + y, color);
                draw_pixel(ctx, x0 - x, y0 + y, color);
                draw_pixel(ctx, x0 + x, y0 - y, color);
                draw_pixel(ctx, x0 - x, y0 - y, color);
                draw_pixel(ctx, x0 + y, y0 + x, color);
                draw_pixel(ctx, x0 - y, y0 + x, color);
                draw_pixel(ctx, x0 + y, y0 - x, color);
                draw_pixel(ctx, x0 - y, y0 - x, color);
        }
}

void draw_circle_filled(context ctx, int x0, int y0, int radius, int color)
{
        /* Source: https://stackoverflow.com/a/14976268/13378037 */

        int x = radius;
        int y = 0;
        int xChange = 1 - (radius << 1);
        int yChange = 0;
        int radiusError = 0;

        while (x >= y) {
                for(int i = x0 - x; i <= x0 + x; i++) {
                        draw_pixel(ctx, i, y0 + y, color);
                        draw_pixel(ctx, i, y0 - y, color);
                }
                for(int i = x0 - y; i <= x0 + y; i++) {
                        draw_pixel(ctx, i, y0 + x, color);
                        draw_pixel(ctx, i, y0 - x, color);
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

void draw_monochrome_bitmap(context ctx, int x, int y, int *bitmap, int color)
{
        for(int i=0; i < 32; i++) {
                for(int j=0; j < 32; j++) {
                        if(bitmap[j+i*32] == 1) {
                                draw_pixel(ctx, x+j, y+i, color);
                        } else {
                                continue;
                        }
                }
        }
}

void draw_char(context ctx, int x, int y, char c, int color)
{
	for(int i=0; i < 32; i++) {
                for(int j=0; j < 32+2; j++) {
                        if(font[c][j+i*32] == 1) {
                                draw_pixel(ctx, x+j, y+i, color);
                        } else {
                                continue;
                        }
                }
        }
}

void draw_string(context ctx, int x, int y, char* str, int color)
{
        int j=0;
        for(int i=0; str[i] != '\0'; i++) {
		draw_char(ctx, x+j, y, str[i], color);
                j+=15;
        }
}

void draw_image(context ctx, int x, int y, int w, int h, int data[])
{
        int i,j;

        for (i=0; i<h; i++) {
                for (j=0; j<w; j++) {
                        int color24 = data[i*w+j];
                        int color16;
                        color16 = (((color24&0xf80000)>>8) + ((color24&0xfc00)>>5) + ((color24&0xf8)>>3));
                        draw_pixel(ctx, x+j, y+i, color16);
                }
        }
}

void draw_image_transparent(context ctx, int x, int y, int w, int h, int data[])
{
        int i,j;

        for (i=0; i<h; i++) {
                for (j=0; j<w; j++) {
                        if (data[i*w+j] == 0x20304) continue;

                        int color24 = data[i*w+j];
                        int color16;
                        color16 = (((color24&0xf80000)>>8) + ((color24&0xfc00)>>5) + ((color24&0xf8)>>3));
                        draw_pixel(ctx, x+j, y+i, color16);
                }
        }
}

