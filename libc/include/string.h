#ifndef _string_h
#define _string_h

#include <stddef.h>
#include <stdarg.h>

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
size_t strlen(const char*);
size_t strsplit(char *str, char c);
int strcmp(char *str1, char *str2);
void strfmt(char *str, char* format, ...);

#endif
