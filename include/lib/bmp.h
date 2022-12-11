#ifndef _bmp_h
#define _bmp_h

typedef struct bmp_header_t {
    uint16_t identifier;
    uint32_t size;
    uint8_t reserved[4];
    uint32_t offset;
} __attribute__((packed)) bmp_header_t;

#endif