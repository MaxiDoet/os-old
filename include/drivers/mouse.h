#ifndef _mouse_h
#define _mouse_h

struct mouse_event {
	int x;
	int y;
	bool button1_pressed;
	bool button2_pressed;
	bool button3_pressed;
};

void mouse_init();
void mouse_irq_handler();
void mouse_add_callback(void (*callback)(struct mouse_event));
void mouse_remove_callback();

#endif
