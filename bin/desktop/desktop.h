#ifndef _desktop_h
#define _desktop_h

#include "../include/kernel/multiboot.h"

typedef struct {
	char* title;
	int width;
	int height;
	int x;
	int y;
	int background;
} window;

void* fb;
void* bb;

int fb_width;
int fb_height;
int fb_pitch;

void desktop_init(unsigned long fbaddr, int width, int height, int pitch);

#endif
