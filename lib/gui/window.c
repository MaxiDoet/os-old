#include <stdint.h>
#include <stdbool.h>

#include "window.h"
#include "context.h"
#include "direct.h"

void draw_window(context ctx, window win)
{
	// Frame
	draw_rounded_rectangle(ctx, win.x, win.y, win.width, 30, 5, FRAME_COLOR);

	// Content
	draw_rounded_rectangle(ctx, win.x, win.y + 20, win.width, win.height - 20, 5, WINDOW_COLOR);

	// Frame buttons
	draw_circle_filled(ctx, win.x + win.width - 40, win.y + 8, 5, 0x5ECC);
	draw_circle_filled(ctx, win.x + win.width - 25, win.y + 8, 5, 0xFE26);
	draw_circle_filled(ctx, win.x + win.width - 10, win.y + 8, 5, 0xF28A);
}
