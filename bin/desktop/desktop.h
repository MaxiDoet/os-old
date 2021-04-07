#ifndef _desktop_h
#define _desktop_h

#include "../include/kernel/multiboot.h"

typedef struct {
	char* title;
	int width;
	int height;
	int x;
	int y;
} window;

void desktop_init(void *fb, multiboot_info_t *mbi);

#endif
