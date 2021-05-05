#ifndef _vga_h
#define _vga_h

#include "vga-color.h"

size_t vgacon_width;
size_t vgacon_height;

size_t vgacon_x;
size_t vgacon_y;

enum vga_color vgacon_text_fg_color;
enum vga_color vgacon_text_bg_color;

uint16_t* vgacon_mem;

void vgacon_init();
void vgacon_text_put(enum vga_color fg, enum vga_color bg, size_t x, size_t y, char c);
void vgacon_set_color(enum vga_color fg, enum vga_color bg);

void vgacon_clear(enum vga_color color);

#endif
