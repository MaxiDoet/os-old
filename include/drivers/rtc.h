#ifndef _rtc_h
#define _rtc_h

#define RTC_PORT_ADDRESS 0x70
#define RTC_PORT_DATA 0x71

uint8_t rtc_read(uint8_t offset);
void rtc_write(uint8_t offset, uint8_t val);
uint8_t rtc_bcd_decode(uint8_t bcd);

#endif
