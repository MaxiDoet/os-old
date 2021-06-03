#ifndef _gui_context_h
#define _gui_context_h

typedef struct context {
	int x;
	int y;
	int width;
	int height;
	void *fb;
	uint16_t fb_pitch;
	int font[][1024];
} context;

#endif
