#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/mouse.h"
#include "../include/kernel/asm.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/irq.h"

static int mouse_callback_num = 0;
static void (*mouse_callbacks[8])(struct mouse_event);
static struct mouse_event event;

static bool button1_pressed;
static bool button2_pressed;
static bool button3_pressed;

void mouse_fire_callback(struct mouse_event event)
{
	for(int i=0; mouse_callbacks[i]; i++) {
               	mouse_callbacks[i](event);
        }
}

void mouse_add_callback(void (*callback)(struct mouse_event))
{
        mouse_callbacks[mouse_callback_num] = callback;
        mouse_callback_num++;
}

void mouse_remove_callback()
{
        mouse_callbacks[mouse_callback_num] = 0;
}

void mouse_irq_handler()
{
	static uint16_t mouse_receive_byte[3];
        static uint8_t mouse_receive_counter=0;

	uint8_t data = inb(0x60);

	switch(mouse_receive_counter) {
		case 0:
	 		// Flags, buttons
			mouse_receive_byte[0] = data;
			mouse_receive_counter++;

			button1_pressed = mouse_receive_byte[0] & 0x1 ? true : false;
			button2_pressed = mouse_receive_byte[0] & (0x1 << 1) ? true : false;
			button3_pressed = mouse_receive_byte[0] & (0x1 << 2) ? true : false;

			break;
		case 1:
			// X Movement
			mouse_receive_byte[1] = data;
			mouse_receive_counter++;
			break;
		case 2:
			// Y Movement
			mouse_receive_byte[2] = data;

			event.x = mouse_receive_byte[1] - ((mouse_receive_byte[0] << 4) & 0x100);
			event.y = mouse_receive_byte[2] - ((mouse_receive_byte[0] << 3) & 0x100);

			event.button1_pressed = button1_pressed;
			event.button2_pressed = button2_pressed;
			event.button3_pressed = button3_pressed;

			mouse_fire_callback(event);

			mouse_receive_counter=0;
			break;
	}
}

void mouse_write(uint8_t data)
{
	// Tell controller we're sending a command to the mouse
	outb(0x64, 0xD4);
	outb(0x60, data);

	while (inb(0x60) != 0xFA) {
		// Wait for ack
	}
}

void mouse_init()
{
	// Enable auxiliary mouse device
	outb(0x64, 0xA8);

	// Enable irq
        outb(0x64, 0x20);

        uint8_t status = inb(0x60) | 2;
        outb(0x64, 0x60);
        outb(0x60, status);

	// Use default settings
	mouse_write(0xF6);

	// Enable mouse
	mouse_write(0xF4);

        inb(0x60);

	irq_install_handler(12, mouse_irq_handler);
}

