#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/rtc.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/io.h"

uint8_t rtc_read(uint8_t offset) {
  uint8_t tmp = inb(RTC_PORT_ADDRESS);
  outb(RTC_PORT_ADDRESS, (tmp & 0x80) | (offset & 0x7F));
  return inb(RTC_PORT_DATA);
}

void rtc_write(uint8_t offset,uint8_t val) {
  uint8_t tmp = inb(RTC_PORT_ADDRESS);
  outb(RTC_PORT_ADDRESS, (tmp & 0x80) | (offset & 0x7F));
  outb(RTC_PORT_DATA,val);
}

uint8_t rtc_bcd_decode(uint8_t bcd)
{
	return (bcd & 0xF) + ((bcd / 16) * 10);
}

void rtc_print_time_date()
{
	/* Note: Later this should return time_t */
	int hours = rtc_bcd_decode(rtc_read(0x04));
	int minutes = rtc_bcd_decode(rtc_read(0x02));
	int seconds = rtc_bcd_decode(rtc_read(0x00));

	int month = rtc_bcd_decode(rtc_read(0x08));
	int day = rtc_bcd_decode(rtc_read(0x07));
	int year = rtc_bcd_decode(rtc_read(0x09));

	if (hours < 10) kdebug("0");
	kdebug("%d:", hours);

	if (minutes < 10) kdebug("0");
	kdebug("%d:", minutes);

	if (seconds < 10) kdebug("0");
	kdebug("%d ", seconds);

	if (month < 10) kdebug("0");
	kdebug("%d/", month);

	if (minutes < 10) kdebug("0");
	kdebug("%d/", day);

	if (year < 10) kdebug("0");
	kdebug("%d\r\n", year);
}
