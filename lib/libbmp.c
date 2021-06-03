#include <stdint.h>

#include "../lib/include/libbmp.h"
#include "../include/kernel/kernel.h"

uint8_t bmp_parse(uint16_t *buf)
{
	struct bmp_header *header = (struct bmp_header *) buf;
	if (header->bf_type != 0x4D42) return -1;

	struct bmp_infoblock *infoblock = (struct bmp_infoblock *) ((uint32_t) buf + 14);

	for (int i=0; i < 50; i++) {
		kdebug("%x ", buf[i]);
	}

	kdebug("BMP Info:\r\n");
	kdebug("Width: %dpx\r\nHeight: %dpx\r\nPlanes: %d\r\n", infoblock->bi_height, infoblock->bi_height, infoblock->bi_planes);
}

