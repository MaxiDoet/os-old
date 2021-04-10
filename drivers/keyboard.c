#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/keyboard.h"
#include "../include/kernel/asm.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/irq.h"
#include "../include/lib/print.h"

static void (*keyboard_callbacks[8])(struct keyboard_event);
static bool shift_pressed;
static struct keyboard_event event;

void keyboard_fire_callback(enum keyboard_key key, bool released)
{
	kdebug("keyboard: fire callback\r\n");
	if (key == KEY_LSHIFT || key == KEY_RSHIFT) {
		shift_pressed = !released;
	}

	event.key = key;
	event.released = released;
	event.shift_pressed = shift_pressed;

	for(int i=0; keyboard_callbacks[i]; i++) {
		keyboard_callbacks[i](event);
	}
}

void keyboard_add_callback(void (*callback)(struct keyboard_event))
{
	keyboard_callbacks[0] = callback;
}

void keyboard_irq_handler()
{
	uint8_t scancode = inb(0x60);

	if (!keyboard_scancodeset) return;

	if((scancode & 128) == 128) {
		// Released
		keyboard_fire_callback(keyboard_scancodeset[scancode], false);
	} else {
		// Pressed
		keyboard_fire_callback(keyboard_scancodeset[scancode], false);
	}
}

void keyboard_init() {
	irq_install_handler(1, keyboard_irq_handler);
}


