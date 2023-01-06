#ifndef _mouse_h
#define _mouse_h

typedef struct mouse_event_t {
	int x;
	int y;

	bool button_left_pressed;
	bool button_right_pressed;
	bool button_middle_pressed;
} mouse_event_t;

void mouse_init();
void mouse_irq_handler();
void mouse_add_callback(void (*callback)(struct mouse_event_t));
void mouse_remove_callback();

#endif
