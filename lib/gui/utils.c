#include <stdint.h>

#include "../kernel/mem/heap.h"
#include "../libc/include/string.h"

#include <gui/utils.h>

void bitmap_flip_h(bitmap_t *bitmap)
{
    uint8_t *bitmap_copy = (uint8_t *) malloc(bitmap->width * bitmap->height * bitmap->bpp / 8);
    memcpy(bitmap_copy, bitmap->data, bitmap->width * bitmap->height * bitmap->bpp / 8);

    for (uint16_t y=0; y < bitmap->height; y++) {
        for (uint16_t x=0; x < bitmap->width; x++) {
            memcpy(&bitmap->data[(bitmap->width * y + x) * bitmap->bpp / 8], &bitmap_copy[((bitmap->height - y) * bitmap->width + x) * bitmap->bpp / 8], bitmap->bpp / 8);
        }
    }

    free(bitmap_copy);
}

uint32_t rgb565_to_rgb888(uint32_t rgb565)
{
    uint32_t rgb888 = 0;

    rgb888 |= ((rgb565 & 0x1F) * 255 + 15) / 31;
    rgb888 |= ((((rgb565 & 0x7E0) >> 5) * 255 + 31) / 63) << 8;
    rgb888 |= ((((rgb565 & 0xF800) >> 11) * 255 + 31) / 31) << 16;

    return rgb888;
}

uint32_t rgb888_to_rgb565(uint32_t rgb888)
{
	uint32_t rgb565 = 0;

    rgb565 |= (rgb888 & 0xFF >> 3) & 0x1F;
    rgb565 |= ((((rgb888 & 0xFF00) >> 8) >> 2) & 0x3F) << 5;
    rgb565 |= ((((rgb888 & 0xFF0000) >> 16) >> 3) & 0x1F) << 11;

	return rgb565;
}