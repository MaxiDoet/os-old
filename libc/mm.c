#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../libc/include/mm.h"

void mm_init(size_t start, size_t size)
{
	mm_t manager;

	if (size < sizeof(mm_chunk)) {
		manager.first_chunk = 0;
	} else {
		manager.first_chunk = (mm_chunk*) start;

		manager.first_chunk->allocated = false;
		manager.first_chunk->next_chunk = 0;
		manager.first_chunk->previous_chunk = 0;
		manager.first_chunk->size = size - sizeof(mm_chunk);
	}

	mm = manager;
}

void* malloc(mm_t manager, size_t size)
{
	mm_chunk* result = 0;

	for (mm_chunk* chunk = manager.first_chunk; chunk != 0 && result == 0; chunk = chunk->next_chunk) {
		if (chunk->size > size && !chunk->allocated) {
			result = chunk;
		}
	}

	if (result == 0) return 0;

	if (result->size >= size + sizeof(mm_chunk) + 1) {
		mm_chunk* temp = (mm_chunk*)((size_t)result + sizeof(mm_chunk) + size);

		temp->allocated = false;
		temp->size = result->size - size - sizeof(mm_chunk);
		temp->next_chunk = result->next_chunk;
		temp->previous_chunk = result;

		if (temp->next_chunk != 0) {
			temp->next_chunk = temp;
			temp->previous_chunk = temp;
		}

		result->size = size;
		result->next_chunk = temp;
	}

	result->allocated = true;

	return (void*)(((size_t)result) + sizeof(mm_chunk));
}

void free(mm_t manager, void* ptr)
{

}

