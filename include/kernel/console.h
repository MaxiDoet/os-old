#ifndef _kernel_console_h
#define _kernel_console_h

#define CONSOLE_COLOR_BLACK         0x00
#define CONSOLE_COLOR_BLUE          0x01
#define CONSOLE_COLOR_GREEN         0x02
#define CONSOLE_COLOR_CYAN          0x03
#define CONSOLE_COLOR_RED           0x04
#define CONSOLE_COLOR_MAGENTA       0x05
#define CONSOLE_COLOR_BROWN         0x06
#define CONSOLE_COLOR_LIGHT_GRAY    0x07
#define CONSOLE_COLOR_WHITE         0x0F

void console_init();
void console_putc(uint8_t x, uint8_t y, uint16_t attributes, char c);
void console_write(char c);

#endif