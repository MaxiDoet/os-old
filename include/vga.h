#ifndef _vga_h
#define _vga_h

/* Here we define all colours for the specific mode */

/* Text mode 80x25 */
enum vga_text_color {
        VGA_TEXT_COLOR_BLACK = 0,
        VGA_TEXT_COLOR_BLUE = 1,
        VGA_TEXT_COLOR_GREEN = 2,
        VGA_TEXT_COLOR_CYAN = 3,
        VGA_TEXT_COLOR_RED = 4,
        VGA_TEXT_COLOR_PURPLE = 5,
        VGA_TEXT_COLOR_BROWN = 6,
        VGA_TEXT_COLOR_GRAY = 7,
        VGA_TEXT_COLOR_DARKGRAY = 8,
        VGA_TEXT_COLOR_LIGHTBLUE = 9,
        VGA_TEXT_COLOR_LIGHTGREEN = 10,
        VGA_TEXT_COLOR_LIGHTCYAN = 11,
        VGA_TEXT_COLOR_LIGHTRED = 12,
        VGA_TEXT_COLOR_LIGHTPURPLE = 13,
        VGA_TEXT_COLOR_YELLOW = 14,
        VGA_TEXT_COLOR_WHITE = 15
};

enum vga_mode {
        VGA_TEXT_MODE
};

size_t vga_width;
size_t vga_height;

size_t vga_x;
size_t vga_y;

enum vga_text_color vga_text_fg_color;
enum vga_text_color vga_text_bg_color;

uint16_t* vga_mem;

void vga_init(enum vga_mode mode);
void vga_text_put(enum vga_text_color fg, enum vga_text_color bg, size_t x, size_t y, char c);
void vga_set_color(enum vga_text_color fg, enum vga_text_color bg);

void vga_clear(enum vga_text_color color);

#endif
