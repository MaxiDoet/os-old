#ifndef _gui_direct_h
#define _gui_direct_h

#include "context.h"

void draw_pixel(context ctx, int x, int y, uint16_t color);
void draw_horizontal_line(context ctx, int x, int y, int w, uint16_t color);
void draw_vertical_line(context ctx, int x, int y, int h, uint16_t color);
void draw_line(context ctx, int x0, int y0, int x1, int y1, uint16_t color);
void draw_filled_rectangle(context ctx, int x, int y, int w, int h, uint16_t color);
void draw_rectangle(context ctx, int x, int y, int w, int h, int size, uint16_t color);
void draw_rounded_rectangle(context ctx, int x, int y, int w, int h, int r, uint16_t color);
void draw_circle(context ctx, int x0, int y0, int radius, uint16_t color);
void draw_circle_filled(context ctx, int x0, int y0, int radius, uint16_t color);
void draw_monochrome_bitmap(context ctx, int x, int y, int *bitmap, uint16_t color);
void draw_char(context ctx, int x, int y, char c, uint16_t color);
void draw_string(context ctx, int x, int y, char* str, uint16_t color);
void draw_image(context ctx, int x, int y, int w, int h, uint32_t data[]);
void draw_image_transparent(context ctx, int x, int y, int w, int h, uint32_t data[]);
void draw_bitmap(context ctx, int x, int y, int w, int h, uint16_t *data);

#endif
