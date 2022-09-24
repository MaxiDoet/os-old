#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../libc/include/mm.h"

typedef struct mm_chunk_t {
	struct mm_chunk_t *next_chunk;
	struct mm_chunk_t *previous_chunk;
	bool allocated;
	bool dirty;
	uint32_t size;
} __attribute__((packed)) mm_chunk_t;

static mm_chunk_t *first_chunk;

void mm_init(uint32_t start, size_t size)
{
	first_chunk = (mm_chunk_t *) start;

	first_chunk->next_chunk = 0;
	first_chunk->previous_chunk = 0;
	first_chunk->allocated = false;
	first_chunk->dirty = false;
	first_chunk->size = size - sizeof(mm_chunk_t);
}

void* malloc(uint32_t size)
{
	mm_chunk_t *current = first_chunk;
	mm_chunk_t *last = 0;
	mm_chunk_t *result = 0;

	#ifdef MM_DEBUG
	kdebug("[malloc] requested %d bytes\r\n", size);
	#endif

	while (!result) {

		kdebug("[malloc] current: %x | next: %x | allocated: %s | dirty: %s | size: %d\r\n",
			(uint32_t) current,
			(uint32_t) current->next_chunk,
			current->allocated ? "Yes" : "No",
			current->dirty ? "Yes" : "No",
			current->size
		);

		if (current->allocated) {
			current->next_chunk = (mm_chunk_t *) ((uint32_t) current + sizeof(mm_chunk_t) + current->size);
			current = current->next_chunk;
			continue;
		}

		if (!current->dirty) {
			current->size = size;
			current->dirty = true;
			current->next_chunk = (mm_chunk_t *) ((uint32_t) current + sizeof(mm_chunk_t) + current->size);
		}

		if (current->size < size) {
			current = current->next_chunk;
			continue;
		}

		#ifdef MM_DEBUG
		kdebug("[malloc] found unallocated chunk %x | size: %d\r\n", (uint32_t) current, current->size);
		#endif

		current->allocated = true;
		result = current;

		/*
		if (!current->allocated) {

			if (current->size != 0) {
				// Check size
				if (current->size < size) {
					current = current->next_chunk;
					continue;
				}
			} else {
				// This is the first time this chunks gets allocated so mark it as dirty
				kdebug("unallocated\r\n");
				current->size = size;
				current->dirty = true;
				current->next_chunk = (mm_chunk_t *) ((uint32_t) current + sizeof(mm_chunk_t) + current->size);
			}

			#ifdef MM_DEBUG
			kdebug("[malloc] found unallocated chunk %x | size: %d\r\n", (uint32_t) current, current->size);
			#endif

			current->allocated = true;

			result = current;

		} else {
			current = current->next_chunk;
			//last = current;
		}
		*/
	}

	return (void *) ((uint32_t) result + sizeof(mm_chunk_t));
}

void free(void* ptr)
{
	mm_chunk_t *chunk = (mm_chunk_t *) ((uint32_t) ptr - sizeof(mm_chunk_t));

	kdebug("[free] chunk: %x next: %x allocated: %s size: %d bytes\r\n",
		(uint32_t) chunk,
		(uint32_t) chunk->next_chunk,
		chunk->allocated ? "Yes" : "No",
		chunk->size
		);

	chunk->allocated = false;
	
}

