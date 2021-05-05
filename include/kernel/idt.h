#ifndef _idt_h
#define _idt_h

#include <stdint.h>

#define IDT_ENTRIES 256

struct idt_entry {
    uint16_t base_lower;
    uint16_t selector;
    uint8_t unused;
    uint8_t type_attributes;
    uint16_t base_higher;
} __attribute__((packed));

struct idt_description {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_description idt_desc;
struct idt_entry idt[IDT_ENTRIES];

void idt_set_gate(unsigned int pos, uint32_t base, uint16_t selector, uint8_t type_attributes);
void idt_install();

#endif
