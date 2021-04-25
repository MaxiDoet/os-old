#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../libc/include/string.h"
#include "../include/kernel/pmm.h"
#include "../include/kernel/kernel.h"
#include "../config.h"

static uint8_t *bitmap;
uint32_t bitmap_size;

/* These are defined in the linker.ld file */
extern const void kernel_start;
extern const void kernel_end;

void pmm_free(uint32_t index);
void pmm_mark(uint32_t index);

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

	kdebug("Bitmap Size: %d\r\n", bitmap_size);
	bitmap = (uint8_t*) (&kernel_end);

	for (int i=0; i<bitmap_size; i++) {
		bitmap[i] = 0;
	}

}

uint32_t pmm_alloc()
{
	for (int i=0; i < bitmap_size; i++) {
		kdebug("Page: %d |%s|\r\n", i, (bitmap[i]==1) ? "x" : " ");

		if (bitmap[i] == 0) {
			// Set page to allocated
			bitmap[i] = 1;
			return i;
		}
	}

	// Out of Memory
	kpanic("Out of Memory!");
}

void pmm_free(uint32_t index)
{
	bitmap[index] = 0;
}

void pmm_mark(uint32_t index)
{
	bitmap[index] = 1;
}
