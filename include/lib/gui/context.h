#ifndef _gui_context_h
#define _gui_context_h

#include <stdbool.h>

#include "window.h"

typedef struct context {
	int x;
	int y;
	int width;
	int height;
	
	void *fb;
	uint16_t fb_pitch;
	
	bool grabbing;
	window *grabbed_window;

	int font[][1024];
} context;

#endif
