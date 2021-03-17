#ifndef _serial_h
#define _serial_h

#define SERIAL_DATA_PORT 0x3F8

void serial_init();
int serial_avaiable();
uint8_t serial_read();
int serial_transmit_empty();
void serial_write(char c);

#endif
