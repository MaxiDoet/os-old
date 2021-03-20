#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/keyboard.h"
#include "../include/asm.h"
#include "../include/print.h"

uint8_t keyboard_read()
{
	while(1) {
		if (inb(0x64) & 0x1) {
			return inb(0x60);
		}
	}
}
