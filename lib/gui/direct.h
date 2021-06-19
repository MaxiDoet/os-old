#ifndef _gui_direct_h
#define _gui_direct_h

#include "context.h"

void draw_pixel(context ctx, int x, int y, int color);
void draw_horizontal_line(context ctx, int x, int y, int w, int color);
void draw_vertical_line(context ctx, int x, int y, int h, int color);
void draw_line(context ctx, int x0, int y0, int x1, int y1, int color);
void draw_filled_rectangle(context ctx, int x, int y, int w, int h, int color);
void draw_rectangle(context ctx, int x, int y, int w, int h, int size, int color);
void draw_rounded_rectangle(context ctx, int x, int y, int w, int h, int r, int color);
void draw_circle(context ctx, int x0, int y0, int radius, int color);
void draw_circle_filled(context ctx, int x0, int y0, int radius, int color);
void draw_monochrome_bitmap(context ctx, int x, int y, int *bitmap, int color);
void draw_char(context ctx, int x, int y, char c, int color);
void draw_string(context ctx, int x, int y, char* str, int color);
void draw_image(context ctx, int x, int y, int w, int h, int data[]);
void draw_image_transparent(context ctx, int x, int y, int w, int h, int data[]);

#endif
