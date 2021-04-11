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

static int mouse_x;
static int mouse_y;

void mouse_fire_callback()
{
	event.x = mouse_x;
	event.y = mouse_y;

	/*
	event.button1_pressed = mouse_receive_byte[0] & 0x1 ? true : false;
	event.button2_pressed = mouse_receive_byte[0] & (0x1 << 1) ? true : false;
	event.button3_pressed = mouse_receive_byte[0] & (0x1 << 2) ? true : false;
	*/

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
			break;
		case 1:
			// X Movement
			mouse_receive_byte[1] = data;
			mouse_receive_counter++;
			break;
		case 2:
			// Y Movement
			mouse_receive_byte[2] = data;

			/*
			mouse_x += mouse_receive_byte[1] | 0;
			mouse_y -= mouse_receive_byte[2] | 0;
			*/

			mouse_x += mouse_receive_byte[1] - ((mouse_receive_byte[0] << 4) & 0x100);
			mouse_y -= mouse_receive_byte[2] - ((mouse_receive_byte[0] << 3) & 0x100);

			/*
			if (mouse_x < 0)
				mouse_x = 0;
			if (mouse_y < 0)
				mouse_y = 0;
			if (mouse_x >= 799)
				mouse_x = 799;
			if (mouse_y >= 599)
				mouse_y = 599;
			*/

			mouse_fire_callback();

			mouse_receive_counter=0;
			break;
	}
}

void mouse_init()
{
	outb(0x64, 0xA8);
        outb(0x64, 0x20);
        uint8_t status = inb(0x60) | 2;
        outb(0x64, 0x60);
        outb(0x60, status);

	// Tell the ps2 controller we're using the second port
        outb(0x64, 0xD4);
	// Enable packet streaming
        outb(0x60, 0xF4);

        inb(0x60);

	irq_install_handler(12, mouse_irq_handler);
}

