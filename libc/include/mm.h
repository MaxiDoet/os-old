#ifndef _mm_h
#define _mm_h

typedef struct mm_chunk {
	struct mm_chunk *next_chunk;
	struct mm_chunk *previous_chunk;
	bool allocated;
	size_t size;
} mm_chunk;

typedef struct {
	mm_chunk* first_chunk;
} mm_t;

mm_t mm;

void mm_init(size_t start, size_t size);
void* malloc(mm_t manager, size_t size);
void free(mm_t manager, void* ptr);

#endif
