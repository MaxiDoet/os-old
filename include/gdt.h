#ifndef _gdt_h
#define _gdt_h

uint32_t gdt[4];

void gdt_init();
void gdt_set_entry(int i, unsigned int base, unsigned int limit, int flags);

#endif
