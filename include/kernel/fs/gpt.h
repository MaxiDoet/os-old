#ifndef _fs_gpt_h
#define _fs_gpt_h

#include <stdint.h>

#define GPT_ENTRY_ATTRIBUTE_SYSTEM      (1 << 0)
#define GPT_ENTRY_ATTRIBUTE_BOOTABLE    (1 << 2)
#define GPT_ENTRY_ATTRIBUTE_READONLY    (1 << 60)
#define GPT_ENTRY_ATTRIBUTE_HIDDEN      (1 << 62)

typedef struct gpt_table_header_t {
    uint8_t signature[8];
    uint8_t revision[4];
    uint32_t size;
    uint32_t header_checksum;
    uint8_t reserved[4];
    uint64_t header_lba;
    uint64_t backup_lba;
    uint64_t start_lba;
    uint64_t last_lba;
    uint8_t guid[16];
    uint64_t table_lba;
    uint32_t table_entry_count;
    uint32_t table_entry_size;
    uint32_t table_checksum;
} __attribute__((packed)) gpt_table_header_t;

typedef struct gpt_table_entry_t {
    uint8_t type_guid[16];
    uint8_t guid[16];
    uint64_t start_lba;
    uint64_t last_lba;
    uint64_t attributes;
    uint8_t name[72];
} __attribute__((packed)) gpt_table_entry_t;

#endif