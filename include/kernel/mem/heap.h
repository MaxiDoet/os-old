#ifndef _kernel_mem_heap_h
#define _kernel_mem_heap_h

//#define HEAP_DEBUG

void heap_init(uint32_t start, uint32_t size);
void* malloc(uint32_t size);
void free(void* ptr);

#endif
