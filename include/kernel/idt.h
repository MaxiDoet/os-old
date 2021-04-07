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

struct cpu_state {
    uint32_t   eax;
    uint32_t   ebx;
    uint32_t   ecx;
    uint32_t   edx;
    uint32_t   esi;
    uint32_t   edi;
    uint32_t   ebp;

    uint32_t   intr;

    uint32_t   eip;
    uint32_t   cs;
    uint32_t   eflags;
    uint32_t   esp;
    uint32_t   ss;
};

struct idt_description idt_desc;
struct idt_entry idt[IDT_ENTRIES];

void idt_set_gate(unsigned int pos, uint32_t base, uint16_t selector, uint8_t type_attributes);
void idt_install();

#endif
