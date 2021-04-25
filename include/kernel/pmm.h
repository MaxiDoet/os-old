#ifndef _pmm_h
#define _pmm_h

#include "../include/kernel/multiboot.h"

void pmm_init(multiboot_info_t *mbi);
uint32_t pmm_alloc();
void pmm_free(uint32_t index);

#endif
