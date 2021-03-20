#ifndef _keyboard_h
#define _keyboard_h

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_COMMAND_PORT 0x64

uint8_t keyboard_read();

#endif
