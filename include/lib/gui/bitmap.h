#ifndef _gui_bitmap_h
#define _gui_bitmap_h

typedef struct bitmap_t {
    uint16_t width;
    uint16_t height;
    uint8_t bpp;

    uint8_t *data;
} bitmap_t;

#endif