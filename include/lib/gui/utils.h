#ifndef _gui_utils_h
#define _gui_utils_h

#include <gui/bitmap.h>

void bitmap_flip_h(bitmap_t *bitmap);
//inline uint32_t rgb565_to_rgb888(uint32_t color_565);
extern uint32_t rgb888_to_rgb565(uint32_t rgb888);

#endif