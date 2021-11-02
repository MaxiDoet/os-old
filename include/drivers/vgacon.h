#ifndef _vga_h
#define _vga_h

#include "vga-color.h"

static size_t vgacon_width;
static size_t vgacon_height;

static size_t vgacon_x;
static size_t vgacon_y;

static enum vga_color vgacon_text_fg_color;
static enum vga_color vgacon_text_bg_color;

void vgacon_init();
void vgacon_text_put(enum vga_color fg, enum vga_color bg, size_t x, size_t y, char c);
void vgacon_set_color(enum vga_color fg, enum vga_color bg);

void vgacon_clear(enum vga_color color);

#endif
