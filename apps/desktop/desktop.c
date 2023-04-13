#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../apps/desktop/desktop.h"
#include "../apps/desktop/font.h"

#include "../include/kernel/kernel.h"
#include "../include/kernel/mem/heap.h"
#include "../include/kernel/audio/dev.h"
#include "../libc/include/string.h"

#include "../include/drivers/ac97.h"
#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"
#include "../include/drivers/serial.h"
#include "../include/drivers/rtc.h"

#include "../config.h"

#include <gui/surface.h>
#include <gui/direct.h>
#include <gui/bitmap.h>
#include <gui/utils.h>
#include <bmp.h>

#define WINDOW_COLOR 	0x262626
#define TITLE_BAR_COLOR 0x4D4C4C

typedef struct window_t {
	int x;
	int y;
	int width;
	int height;
	
	char *title;

	bool coliding_x;
	bool coliding_y;

	bool focused;
	bool grabbed;
} window_t;

surface_t *screen;
static void *fb;
static void *bb;

static window_t *windows[10];
window_t *grabbed_window;
bool grabbing;
static int cursor_x;
static int cursor_y;

void draw_window(window_t *win)
{
	// Titlebar
	direct_draw_rectangle(screen, win->x, win->y, win->width, 20, TITLE_BAR_COLOR);

	// Title
	//direct_draw_string(screen, win->x + 5, win->y - 8, font, win->title, 0xFFFFFF);

	// Window
	direct_draw_rectangle(screen, win->x, win->y + 20, win->width, win->height - 20, WINDOW_COLOR);
}

static void mouse_handler(mouse_event_t event)
{
	int movement_x = event.x;
	int movement_y = event.y;

	if (!grabbing) {
		cursor_x += movement_x;
		cursor_y -= movement_y;
	}

	if (grabbing && grabbed_window) {
		grabbed_window->x += movement_x;
		grabbed_window->y -= movement_y;
	}

	if (grabbing && grabbed_window) {
		if (!grabbed_window->coliding_x) {
			cursor_x += movement_x;
		}

		if (!grabbed_window->coliding_y) {
			cursor_y -= movement_y;
		}
	}

	// Check for collision
	if (cursor_x < 0) cursor_x = 0;
	if (cursor_x > screen->width) cursor_x = screen->width;
	if (cursor_y < 0) cursor_y = 0;
	if (cursor_y > screen->height) cursor_y = screen->height;

	for (int i=0; i < 1; i++) {
		if (windows[i] == NULL) continue;

		if (!windows[i]->grabbed && event.button_left_pressed && cursor_x >= windows[i]->x-10 && cursor_x <= windows[i]->x-10 + windows[i]->width && cursor_y >= windows[i]->y-10 && cursor_y <= windows[i]->y-10 + 20) {
			windows[i]->grabbed = true;
			windows[i]->focused = true;

			grabbing = true;
			grabbed_window = windows[i];
		}

		if (windows[i]->grabbed && !event.button_left_pressed) {
			windows[i]->grabbed = false;

			grabbing = false;
			grabbed_window = NULL;
		}

		if (windows[i]->x + windows[i]->width >= screen->width) {
			windows[i]->x = screen->width - windows[i]->width;
		}

		if (windows[i]->y + windows[i]->height >= screen->height) {
			windows[i]->y = screen->height - windows[i]->height;
		}

		if (windows[i]->x <= 0) {
			windows[i]->x = 0;
		}

		if (windows[i]->y <= 0) {
			windows[i]->y = 0;
		}

		windows[i]->coliding_x = (windows[i]->x <= 0 || windows[i]->width >= screen->width);
		windows[i]->coliding_y = (windows[i]->y <= 0 || windows[i]->height >= screen->height);
	}
}

void keyboard_handler(struct keyboard_event event)
{

}

static inline void desktop_swap_fb()
{
	__asm__ volatile("rep movsb" : : "D" (bb), "S" (fb), "c" ((screen->pitch * screen->height)));
}

void desktop_init(multiboot_info_t *mbi, fs_t *root_fs)
{
	kdebug("Framebuffer");
	kdebug(" | Width: %d", mbi->framebuffer_width);
	kdebug(" | Height: %d", mbi->framebuffer_height);
	kdebug(" | BPP: %d", mbi->framebuffer_bpp);
	kdebug(" | Pitch: %d\r\n", mbi->framebuffer_pitch);	

	bb = (void *) (uint32_t) mbi->framebuffer_addr;
	fb = (void *) (uint32_t) malloc(mbi->framebuffer_height * mbi->framebuffer_pitch);

	screen = (surface_t *) malloc(sizeof(surface_t));
	screen->width = mbi->framebuffer_width;
	screen->height = mbi->framebuffer_height;
	screen->bpp = mbi->framebuffer_bpp;
	screen->pitch = mbi->framebuffer_pitch;
	screen->fb = fb;

	cursor_x = screen->width / 2;
	cursor_y = screen->height / 2;


	/* Load background image */
	fs_file_t background_file = fs_open(root_fs, "/background.bmp");
	uint32_t background_size = fs_size(background_file);
	uint8_t *background_bmp_buf = (uint8_t *) malloc(background_size);
	fs_read(background_file, background_bmp_buf);

	bmp_header_t *background_bmp_header = (bmp_header_t *) background_bmp_buf;

	uint8_t *background_buf = (uint8_t *) malloc(1024 * 768 * 3);
	memcpy(background_buf, background_bmp_buf + background_bmp_header->offset, 1024 * 768 * 3);

	bitmap_t *background_bitmap = (bitmap_t *) malloc(sizeof(bitmap_t));
	background_bitmap->width = 1024;
	background_bitmap->height = 768;
	background_bitmap->bpp = 24;
	background_bitmap->data = background_buf;
	bitmap_flip_h(background_bitmap);

	/*
	fs_file_t audio_file = fs_open(root_fs, "/audio.wav");
	uint32_t audio_size = fs_size(audio_file);
	uint8_t *audio_buf = (uint8_t *) malloc(audio_size);
	fs_read(audio_file, audio_buf);
	audio_return_t status = audio_dev_play(audio_buf, audio_size);
	free(audio_buf);
	*/

	window_t test;
	test.x = 70;
	test.y = 70;
	test.width = 400;
	test.height = 300;
	test.title = "Control Panel";

	windows[0] = &test;

	keyboard_add_callback(keyboard_handler);
	mouse_add_callback(mouse_handler);

	char *clock_str = (char *) malloc(20);
	memset(clock_str, 0x00, 20);

	while(1) {
		// Background
		//direct_draw_rectangle(screen, 0, 0, screen->width, screen->height, 0x333333);
		direct_draw_bitmap(screen, 0, 0, background_bitmap);

		// Draw clock
		datetime_t datetime;
		datetime = rtc_read_datetime();
		strfmt(clock_str, "%d:%d\r\n", datetime.hour, datetime.minute);
		direct_draw_string(screen, 20, 20, font, clock_str, 0xFFFFFF);

		for (int i=0; i < 10; i++) {
			if (windows[i] == NULL) continue;
			draw_window(windows[i]);
		}

		// Cursor
		direct_draw_rectangle(screen, cursor_x, cursor_y, 10, 10, 0xFFFFFF);

		// Swap frontbuffer and backbuffer
    	desktop_swap_fb();
	}
}

