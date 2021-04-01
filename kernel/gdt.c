#include <stdint.h>
#include "../include/kernel/gdt.h"

extern void gdt_install();

static void gdt_set_entry(unsigned int pos, uint32_t base, uint32_t limit,
        uint8_t access_rights, uint8_t flags)
{
    gdt[pos].base_lower = base & 0xffff;
    gdt[pos].base_middle = (base >> 16) & 0xff;
    gdt[pos].base_higher = (base >> 24) & 0xffff;

    gdt[pos].limit_lower = limit & 0xffff;
    gdt[pos].flags_limit_higher = ((flags << 4) & 0xf0) | ((limit >> 16) & 0x0f); 

    gdt[pos].access_rights = access_rights;
}

void gdt_setup()
{
    gdt_desc.limit = GDT_ENTRIES * sizeof(struct gdt_entry) - 1;
    gdt_desc.base = (uint32_t) (uintptr_t) gdt;

    gdt_set_entry(0, 0, 0, 0, 0);
    gdt_set_entry(1, 0, 0xfffff, 0x9a, 0xcf);
    gdt_set_entry(2, 0, 0xfffff, 0x92, 0xcf);
    
    gdt_install();
}
