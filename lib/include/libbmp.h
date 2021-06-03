#ifndef _libbmp_h
#define _libbmp_h

struct bmp_header {
	uint16_t bf_type;
	uint32_t bf_size;
	uint32_t bf_reserved;
	uint32_t bf_offbits;
} __attribute__((packed)) bmp_header;

struct bmp_infoblock {
	uint32_t bi_size;
	int32_t bi_width;
	int32_t bi_height;
	uint16_t bi_planes;
	uint16_t bi_bit_count;
	uint32_t bi_compression;
	uint32_t bi_size_image;
	int32_t bi_x_pixels_per_meter;
	int32_t bi_y_pixels_per_meter;
	uint32_t bi_clr_used;
	uint32_t bi_clr_important;
} __attribute__((packed)) bmp_infoblock;

uint8_t bmp_parse(uint16_t *buf);

#endif
