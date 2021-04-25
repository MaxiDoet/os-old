#ifndef _pmm_h
#define _pmm_h

#include "../include/kernel/multiboot.h"

void pmm_init(multiboot_info_t *mbi);
uint32_t pmm_alloc_page();
void pmm_free_page(uint32_t page);

#endif
