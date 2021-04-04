#ifndef _string_h
#define _string_h

#include <stdarg.h>

int strlen(char* str);
uint8_t strcmp(char* str1, char* str2);
char *strcat(char *dest, char *src);
char* strfmt(char* format, va_list *argsp);

#endif
