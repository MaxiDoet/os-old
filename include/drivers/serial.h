#ifndef _serial_h
#define _serial_h

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

void serial_init(uint16_t data_port);
uint8_t serial_avaiable(uint16_t data_port);
uint8_t serial_read(uint16_t data_port);
uint8_t serial_transmit_empty(uint16_t data_port);
void serial_write(uint16_t data_port, char c);

#endif
