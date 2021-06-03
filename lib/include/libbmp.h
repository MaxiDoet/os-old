#ifndef _libbmp_h
#define _libbmp_h

struct bitmap_header {
	uint16_t type;
	uint32_t size;
	uint32_t reserved;
	uint32_t offbits;
} __attribute__((packed)) bitmap_header;

#endif
