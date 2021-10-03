#ifndef _gui_widgets_h
#define _gui_widgets_h

typedef struct gui_label {
	int x;
	int y;
	int color;
	char* text;
};

gui_label draw_label(int x, int y, char *text);

#endif
