#ifndef _mm_h
#define _mm_h

//#define MM_DEBUG

void mm_init(size_t start, size_t size);
void* malloc(size_t size);
void free(void* ptr);

#endif
