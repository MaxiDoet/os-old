#ifndef _fs_gpt_h
#define _fs_gpt_h

#include <stdint.h>

typedef struct gpt_table_header_t {
    uint8_t signature[8];
    uint8_t revision[4];
    uint32_t size;
    uint32_t checksum;
    uint8_t reserved[4];
    uint64_t header_sector;
    uint64_t backup_sector;
    uint64_t start_sector;
    uint64_t last_sector;
    uint8_t guid[16];
} __attribute__((packed)) gpt_table_header_t;

#endif