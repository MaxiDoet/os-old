#include <stdint.h>

#include "../kernel/mem/heap.h"
#include "../libc/include/string.h"

#include <gui/utils.h>

void bitmap_flip_h(bitmap_t *bitmap)
{
    uint8_t *bitmap_copy = (uint8_t *) malloc(bitmap->width * bitmap->height * bitmap->bpp);
    memcpy(bitmap_copy, bitmap->data, bitmap->width * bitmap->height * bitmap->bpp);

    for (uint16_t y=0; y < bitmap->height; y++) {
        for (uint16_t x=0; x < bitmap->width; x++) {
            memcpy(&bitmap->data[(bitmap->width * y + x) * bitmap->bpp], &bitmap_copy[((bitmap->height - y) * bitmap->width + x) * bitmap->bpp], bitmap->bpp);
        }
    }

    free(bitmap_copy);
}