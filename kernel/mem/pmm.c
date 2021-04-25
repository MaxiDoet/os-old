#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../libc/include/string.h"
#include "../include/kernel/pmm.h"
#include "../include/kernel/kernel.h"
#include "../config.h"

static uint8_t *bitmap;
size_t bitmap_size;

void pmm_mark_used()
{

}

void pmm_init(multiboot_info_t *mbi)
{
	bitmap_size = (mbi->mem_upper - mbi->mem_lower) / PAGE_SIZE;
	multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mbi->mmap_addr;

	/*
	// Find space for the bitmap
	for (int i=0; i < mbi->mmap_length; i++) {
		if (mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
			if (mmap[i].size >= bitmap_size) {
				bitmap = (uint8_t *) (unsigned long) mmap[i].addr;
				kdebug("[mem] PMM init\r\n      Bitmap Address: %x\r\n", mmap[i].addr);
				return;
			}
		}
	}
	*/

	for (int i=0; i < mbi->mmap_length; i++) {
                if (mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
                        if (mmap[i].size >= bitmap_size) {
                                bitmap = (uint8_t *) (unsigned long) mmap[i].addr;
                                kdebug("[mem] PMM init\r\n      Bitmap Address: %x\r\n", mmap[i].addr);
                                return;
                        }
                }
        }


	kpanic("No space for bitmap!\r\n");

	memset(bitmap, 0, bitmap_size);
}

uint32_t pmm_alloc_page()
{
	bitmap[0] = 1;

	for (int i=0; i < bitmap_size; i++) {
		kdebug("Page: %d |%s|\r\n", i, (bitmap[i]==1) ? "x" : " ");

		if (bitmap[i] == 0) {
			// Set page to allocated
			bitmap[i] = i;
			return i;
		}
	}

	// Out of Memory
	kpanic("Out of Memory!");
}

void pmm_free_page(uint32_t page)
{
	bitmap[page] = 0;
}
