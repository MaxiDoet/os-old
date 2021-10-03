#include <stdint.h>
#include <stdbool.h>

#include <gui/window.h>
#include <gui/context.h>
#include <gui/direct.h>

void draw_window(context ctx, window *win)
{
	// Frame
	draw_filled_rectangle(ctx, win->x, win->y, win->width, 30, FRAME_COLOR);

	// Content
	draw_filled_rectangle(ctx, win->x, win->y + 20, win->width, win->height - 20, WINDOW_COLOR);

	// Frame buttons
	draw_circle_filled(ctx, win->x + win->width - 40, win->y + 8, 5, 0x5ECC);
	draw_circle_filled(ctx, win->x + win->width - 25, win->y + 8, 5, 0xFE26);
	draw_circle_filled(ctx, win->x + win->width - 10, win->y + 8, 5, 0xF28A);
}
