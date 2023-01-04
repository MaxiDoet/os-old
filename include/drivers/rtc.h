#ifndef _rtc_h
#define _rtc_h

#include "../include/lib/datetime.h"

#define RTC_REG_SEC     0x00
#define RTC_REG_MIN     0x02
#define RTC_REG_HOUR    0x04
#define RTC_REG_WEEKDAY 0x06
#define RTC_REG_DOM     0x07
#define RTC_REG_MONTH   0x08
#define RTC_REG_YEAR    0x09
#define RTC_REG_STA_A   0x0A
#define RTC_REG_STA_B   0x0B

uint8_t rtc_read(uint8_t offset);
datetime_t rtc_read_datetime();

#endif
