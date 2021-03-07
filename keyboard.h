#ifndef _keyboard_h
#define _keyboard_h

#include "asm.h"
#include "tty.h"
#include "print.h"

enum keymap {
	KEY_ESCAPE = 0x01,
	KEY_1 = 0x02,
	KEY_2 = 0x03,
	KEY_3 = 0x04,
	KEY_4 = 0x05,
	KEY_5 = 0x06,
	KEY_6 = 0x07,
	KEY_7 = 0x08,
	KEY_8 = 0x09,
	KEY_9 = 0x0A,
	KEY_0 = 0x0B,
	KEY_DASH = 0x0C,
	KEY_EQUAL = 0x0D,
	KEY_BACKSPACE = 0x0E,
	KEY_TAB = 0x0F,
	KEY_Q = 0x10,
	KEY_W = 0x11,
	KEY_E = 0x12,
	KEY_R = 0x13,
	KEY_T = 0x14,
	KEY_Y = 0x15,
	KEY_U = 0x16,
	KEY_I = 0x17,
	KEY_O = 0x18,
	KEY_P = 0x19,
	KEY_BRACKET_OPEN = 0x1A,
	KEY_BRACKET_CLOSED = 0x1B,
	KEY_RETURN = 0x1C,
	KEY_CONTROL_LEFT = 0x1D,
	KEY_A = 0x1E,
	KEY_S = 0x1F,
	KEY_D = 0x20,
	KEY_F = 0x21,
	KEY_G = 0x22,
	KEY_H = 0x23,
	KEY_J = 0x24,
	KEY_K = 0x25
};

char* keyboard_input() {
	// Wait until one key is pressed and return string
	bool reading = true;

	while(reading) {
		if(inb_p(0x64) & 0x1) {
			switch(inb_p(0x60)) {
				case KEY_1:
					return "1";
				case KEY_2:
					return "2";
				case KEY_3:
					return "3";
				case KEY_4:
					return "4";
				case KEY_5:
					return "5";
				case KEY_6:
					return "6";
				case KEY_7:
					return "7";
				case KEY_8:
					return "8";
				case KEY_9:
					return "9";
				case KEY_0:
					return "0";
				case KEY_DASH:
					return "-";
				case KEY_EQUAL:
					return "=";
				case KEY_BACKSPACE:
					// Remove last character
					return "";
				case KEY_TAB:
					// Will change later just for testing
					for (int i=0; i<4; i++) {
						tty_put(VGA_COLOR_BLACK, VGA_COLOR_BLACK, ' ');
					}
					break;
				case KEY_Q:
					return "q";
				case KEY_W:
					return "w";
				case KEY_E:
					return "e";
				case KEY_R:
					return "r";
				case KEY_RETURN:
					return "enter";
			}
		}
	}
}

#endif
