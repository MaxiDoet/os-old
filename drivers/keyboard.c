#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/keyboard.h"
#include "../include/kernel/io/io.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/irq.h"
#include "../include/lib/print.h"
#include "../config.h"

static int keyboard_callback_num = 0;
static void (*keyboard_callbacks[8])(struct keyboard_event);
static keyboard_event event;

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

static char keyboard_shift_map[256] = {
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O' ,'P', 'A', 'S', 'D', 'F',
    'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',

    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',

    '<', '>', '?', '|', '"', ':', '_', '+', ' ', ' ', ' ', '\t', ' ',

    ' ', ' ', ' ', ' ', ' ', ' ', ' ', '}', '{', ' ', ' ', ' ',

    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '*', '+', '.',

    ' '
};

static char keyboard_printable_map[256] = {
    true, true, true, true, true, true, true, true, true, true,
    true, true, true, true, true, true, true, true, true , true, true, true, true, true,
    true, true, true, true, true, true, true, true, true, true, true, true,
    false, false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, true, true, true, false, true, false, false, false,
    false, false, false, false, false, false, false, true, true, false, false, false,
    true, true, true, true, true, true, true, true, true, true, true, true, true, true,
    false
};

void keyboard_fire_callback(keyboard_event event)
{
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
	// Wait until keyboard buffer is empty
	uint8_t status;
	do {
		status = inb(0x64);
	} while ((status & 0x1) == 0);

	uint8_t key = inb(0x60);
	bool pressed = ((key & 128) != 128);

	if (!pressed) {
		key = key & 0x7F;
	}

	event.key = key;
	event.pressed = pressed;

	if (event.key == 0x2a || event.key == 0x36) {
		event.shift = event.pressed;
	}

	if (pressed && event.key < 256) {
		if (event.shift) {
			event.asci = keyboard_shift_map[keyboard_scancodeset[event.key]];
		} else {
			event.asci = keyboard_char_map[keyboard_scancodeset[event.key]];
		}
	}

	event.printable = keyboard_printable_map[keyboard_scancodeset[event.key]];

	#ifdef DEBUG_KEYBOARD_EVENT
		kdebug("[keyboard] %s event: Key: %x Asci: %x Shift: %d\r\n", ((event.pressed) ? "pressed" : "released"), event.key, event.asci, event.shift);
	#endif

	keyboard_fire_callback(event);
}

void keyboard_send(uint8_t command)
{
	// Wait until controller is ready
	while (inb(0x64) & 0x2){

	}

	outb(0x60, command);
}

void keyboard_init() {
	// Empty keyboard buffer
	while (inb(0x64) & 0x1) {
		inb(0x60);
	}

	/* Activate controller
	   Warning: QEMU just puts the controler in a broken state
	*/
	//keyboard_send(0xF4);

	irq_install_handler(1, keyboard_irq_handler);
}


