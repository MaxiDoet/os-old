#ifndef _serial_h
#define _serial_h

void serial_init(int data_port);
int serial_avaiable(int data_port);
uint8_t serial_read(int data_port);
int serial_transmit_empty(int data_port);
void serial_write(int data_port, char c);

#endif
