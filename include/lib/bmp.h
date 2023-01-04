#ifndef _bmp_h
#define _bmp_h

typedef struct bmp_header_t {
    uint16_t identifier;
    uint32_t size;
    uint8_t reserved[4];
    uint32_t offset;
} __attribute__((packed)) bmp_header_t;

typedef struct bmp_dib_header_t {
    uint32_t size;
    uint16_t bitmap_width;
    uint16_t bitmap_height;
    uint16_t color_planes;
    uint16_t bbp;
} __attribute__((packed)) bmp_dib_header_t;

typedef struct bmp_info_t {
    
} bmp_info_t;

#endif