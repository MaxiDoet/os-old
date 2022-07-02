#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/console.h"

uint8_t console_col;
uint8_t console_row;

void console_init()
{
    
}

void console_putc(uint8_t col, uint8_t row, uint16_t attributes, char c)
{
    *(((uint16_t *) 0xB8000) + (row * 80 + col)) = (attributes << 8) | c;
}

void console_clear()
{
    for (uint8_t row = 0; row < 25; row++) {
        for (uint8_t col = 0; col < 80; col++) {
            console_putc(col, row, 0x0000, ' ');
        }
    }
}

void console_write(char c)
{
    if (c == '\r') {
        return;
    }

    if (c == '\n') {
        console_row++;
        console_col = 0;
        return;
    }

    uint16_t attributes = CONSOLE_COLOR_WHITE | (CONSOLE_COLOR_BLACK << 4);
    console_putc(console_col, console_row, attributes, c); 

    if (++console_col == 80) {
        console_col = 0;
        
        if (++console_row == 25) {
            console_row = 0;
        }
    }
}