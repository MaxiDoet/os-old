#ifndef _gui_window_h
#define _gui_window_h

#include "context.h"

#define WINDOW_COLOR 0x31A6
#define FRAME_COLOR 0x2945

typedef struct window {
	int x;
	int y;
	int width;
	int height;
	char *title;
	bool focused;
	bool grabbed;
} window;

void draw_window(context ctx, window *win);

#endif
