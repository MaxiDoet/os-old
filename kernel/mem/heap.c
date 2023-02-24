#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../include/kernel/mem/heap.h"

#define HEAP_MAGIC 0xABCD

typedef struct heap_chunk_t {
	uint16_t magic;
	struct heap_chunk_t *next_chunk;
	struct heap_chunk_t *previous_chunk;
	bool allocated;
	bool dirty;
	uint32_t size;
} __attribute__((packed)) heap_chunk_t;

static heap_chunk_t *first_chunk;
uint32_t max_heap_size = 0;
uint32_t heap_size = 0;

void heap_init(uint32_t start, uint32_t size)
{
	first_chunk = (heap_chunk_t *) start;

	first_chunk->next_chunk = 0;
	first_chunk->previous_chunk = 0;
	first_chunk->allocated = false;
	first_chunk->dirty = false;
	first_chunk->size = 0;

	max_heap_size = size;
}

void* malloc(uint32_t size)
{
	heap_chunk_t *current = first_chunk;
	heap_chunk_t *last = 0;
	heap_chunk_t *result = 0;

	#ifdef HEAP_DEBUG
	kdebug("[heap] request %d bytes\r\n", size);
	#endif

	while (!result) {
		#ifdef HEAP_DEBUG
		kdebug("[heap] current: %x | data: %x | next: %x | allocated: %s | dirty: %s | size: %d\r\n",
			(uint32_t) current,
			(uint32_t) (current + sizeof(heap_chunk_t)),
			(uint32_t) current->next_chunk,
			current->allocated ? "Yes" : "No",
			current->dirty ? "Yes" : "No",
			current->size
		);
		#endif

		if (current->allocated) {
			current->next_chunk = (heap_chunk_t *) ((uint32_t) current + sizeof(heap_chunk_t) + current->size);
			current = current->next_chunk;
			continue;
		}

		// Last link in the chain
		if (!current->dirty) {
			current->magic = HEAP_MAGIC;
			current->size = size;
			current->dirty = true;
			current->next_chunk = (heap_chunk_t *) ((uint32_t) current + sizeof(heap_chunk_t) + current->size);
		}

		if (current->size < size) {
			current = current->next_chunk;
			continue;
		}

		if ((heap_size + size) >= max_heap_size) {
			kpanic("Out of heap");
		}

		#ifdef HEAP_DEBUG
		if (size != 1024) kdebug("[heap] found unallocated chunk %x | size: %d\r\n", (uint32_t) current, current->size);
		#endif

		current->allocated = true;
		result = current;
	}

	return (void *) ((uint32_t) result + sizeof(heap_chunk_t));
}

void free(void* ptr)
{
	heap_chunk_t *chunk = (heap_chunk_t *) ((uint32_t) ptr - sizeof(heap_chunk_t));

	#ifdef HEAP_DEBUG
	kdebug("[heap] free | chunk: %x | next: %x | allocated: %s | size: %d\r\n",
		(uint32_t) chunk,
		(uint32_t) chunk->next_chunk,
		chunk->allocated ? "Yes" : "No",
		chunk->size
		);
	#endif

	// Check if this points to a valid chunk
	if (chunk->magic == HEAP_MAGIC) {
		chunk->allocated = false;
	} else {
		kdebug("[heap] Tried to free invalid chunk\r\n");
	}
}

