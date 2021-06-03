#include <stdint.h>
#include <stdbool.h>

#include "window.h"
#include "context.h"
#include "direct.h"

void draw_window(context ctx, window win)
{
	draw_rounded_rectangle(ctx, win.x, win.y, win.width, win.height, 5, 0x4228);
}
