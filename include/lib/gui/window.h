#ifndef _gui_window_h
#define _gui_window_h

#define WINDOW_COLOR 0x31A6
#define FRAME_COLOR 0x2945

typedef struct window {
	int x;
	int y;
	int width;
	int height;
	
	char *title;

	bool coliding_x;
	bool coliding_y;

	bool focused;
	bool grabbed;
} window;

#endif
