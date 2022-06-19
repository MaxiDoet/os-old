#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../libc/include/mm.h"

typedef struct mm_chunk_t {
	struct mm_chunk_t *next_chunk;
	struct mm_chunk_t *previous_chunk;
	bool allocated;
	uint32_t size;
} __attribute__((packed)) mm_chunk_t;

static mm_chunk_t *first_chunk;

void mm_init(uint32_t start, size_t size)
{
	first_chunk = (mm_chunk_t *) start;

	first_chunk->next_chunk = 0;
	first_chunk->previous_chunk = 0;
	first_chunk->allocated = false;
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
		//kdebug("[malloc] current: %x | next: %x | allocated: %s | size: %d\r\n", (uint32_t) current, (uint32_t) current->next_chunk, current->allocated ? "Yes" : "No", current->size);

		if (!current->allocated) {
			#ifdef MM_DEBUG
			kdebug("[malloc] found unallocated chunk | size: %d\r\n", current->size);
			#endif

			current->allocated = true;
			current->size = size;
			
			current->next_chunk = (mm_chunk_t *) ((uint32_t) current + sizeof(mm_chunk_t) + current->size);
			//current->previous_chunk = last;

			//last->next_chunk = current;

			result = current;

		} else {
			current = current->next_chunk;
			//last = current;
		}
	}

	return (void *) ((uint32_t) result + sizeof(mm_chunk_t));
}

void free(void* ptr)
{

}

