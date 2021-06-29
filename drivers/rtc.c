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
