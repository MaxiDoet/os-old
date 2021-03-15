#ifndef _cmos_h
#define _cmos_h

#define CMOS_PORT_ADDRESS 0x70
#define CMOS_PORT_DATA 0x71

uint8_t cmos_read(uint8_t offset);
void cmos_write(uint8_t offset, uint8_t val);
uint8_t cmos_rtc_decode(uint8_t bcd);

#endif
