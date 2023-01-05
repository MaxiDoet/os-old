#ifndef _gui_direct_h
#define _gui_direct_h

void direct_draw_pixel(surface_t *surface, uint16_t x, uint16_t y, uint16_t color);
void direct_draw_rectangle(surface_t *surface, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void direct_draw_bitmap(surface_t *surface, int x, int y, int width, int height, uint16_t *data);

/*
void direct_draw_line(uint32_t *fb, int x0, int y0, int x1, int y1, uint16_t color);
void direct_draw_rectangle_filled(uint32_t *fb int x, int y, int w, int h, uint16_t color);
void direct_draw_rectangle(uint32_t *fb, int x, int y, int w, int h, int size, uint16_t color);
void direct_draw_circle(uint32_t *fb, int x0, int y0, int radius, uint16_t color);
void direct_draw_circle_filled(uint32_t *fb, int x0, int y0, int radius, uint16_t color);
void direct_draw_bitmap(uint32_t *fb, int x, int y, int w, int h, uint16_t *data);
*/

#endif
