#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/keyboard.h"
#include "../include/kernel/asm.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/irq.h"
#include "../include/lib/print.h"

static int keyboard_callback_num = 0;
static void (*keyboard_callbacks[8])(struct keyboard_event);
static bool shift_pressed;
static struct keyboard_event event;

static char keyboard_char_map[256] = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o' ,'p', 'a', 's', 'd', 'f',
    'g', 'h', 'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', 'n', 'm',

    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',

    ',', '.', '/', '\\', '\'', ';', '-', '=', ' ', ' ', ' ', '\t', ' ',

    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ']', '[', ' ', ' ', ' ',

    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '*', '+', '.',

    ' '
};

void keyboard_fire_callback(enum keyboard_key key, bool released)
{
	if (key == KEY_LSHIFT || key == KEY_RSHIFT) {
		shift_pressed = !released;
	}

	event.key = key;
	event.released = released;
	event.shift_pressed = shift_pressed;

	if (key < 256) {
		event.asci = keyboard_char_map[key];
	}

	for(int i=0; keyboard_callbacks[i]; i++) {
		keyboard_callbacks[i](event);
	}
}

void keyboard_add_callback(void (*callback)(struct keyboard_event))
{
	keyboard_callbacks[keyboard_callback_num] = callback;
	keyboard_callback_num++;
}

void keyboard_remove_callback()
{
	keyboard_callbacks[keyboard_callback_num] = 0;
}

void keyboard_irq_handler()
{
	// Wait until output buffer is empty
	uint8_t status;
	do {
		status = inb(0x64);
	} while ((status & 0x1) == 0);

	uint8_t scancode = inb(0x60);

	if((scancode & 128) == 128) {
		// Released
		keyboard_fire_callback(keyboard_scancodeset[scancode], true);
	} else {
		// Pressed
		keyboard_fire_callback(keyboard_scancodeset[scancode], false);
	}
}

void keyboard_init() {
	irq_install_handler(1, keyboard_irq_handler);
}


