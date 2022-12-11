#include <stdint.h>

#include "../kernel/mem/heap.h"
#include "../libc/include/string.h"

#include <gui/utils.h>

void bitmap_flip_h(int w, int h, int bpp, uint8_t *bitmap)
{
    uint8_t *bitmap_copy = (uint8_t *) malloc(w * h * bpp);
    memcpy(bitmap_copy, bitmap, w * h * bpp);

    for (int y=0; y < h; y++) {
        for (int x=0; x < w; x++) {
            memcpy(&bitmap[(w * y + x) * bpp], &bitmap_copy[((h - y) * w + x) * bpp], bpp);
        }
    }

    free(bitmap_copy);
}