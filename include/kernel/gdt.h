#ifndef _gdt_h
#define _gdt_h

#include <stdint.h>

#define GDT_ENTRIES 3

struct gdt_entry {
    uint16_t limit_lower;
    uint16_t base_lower;
    uint8_t base_middle;
    uint8_t access_rights;
    uint8_t flags_limit_higher;
    uint8_t base_higher;
} __attribute__((packed));

struct gdt_description {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct gdt_entry gdt[GDT_ENTRIES];
struct gdt_description gdt_desc;

void gdt_setup();

#endif
