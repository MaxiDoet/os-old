#ifndef _gui_window_h
#define _gui_window_h

#include "context.h"

typedef struct window {
	int x;
	int y;
	int width;
	int height;
	char *title;
	bool focused;
	bool grabbed;
} window;

void draw_window(context ctx, window win);

#endif
