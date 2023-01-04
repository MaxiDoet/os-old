#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/rtc.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/io/io.h"

#define RTC_PORT_ADDRESS 0x70
#define RTC_PORT_DATA 0x71

#define RTC_STA_B_DM    (1 << 2) /* Indicates if date is in binary or BCD format */
#define RTC_STA_B_2412  (1 << 1) /* Indicates if hour reg is in 12h or 24h format */

uint8_t rtc_read(uint8_t offset) {
  if (offset == RTC_REG_STA_A || offset == RTC_REG_STA_B) {
    outb(RTC_PORT_ADDRESS, offset);
    return inb(RTC_PORT_DATA);
  }

  /* Read status reg first to figure out which dm we use */
  outb(RTC_PORT_ADDRESS, RTC_REG_STA_B);
  uint8_t sta_b = inb(RTC_PORT_DATA);
  uint8_t dm = sta_b & RTC_STA_B_DM;
  uint8_t hour24 = sta_b & RTC_STA_B_2412;

  /* Read the given reg */
  outb(RTC_PORT_ADDRESS, offset);
  uint8_t value = inb(RTC_PORT_DATA);

  if (dm) {
    /* Binary mode */
    return value;
  } else {
    /* BCD mode */
    return (value & 0xF) + ((value / 16) * 10);
  }
}

datetime_t rtc_read_datetime()
{
  datetime_t datetime;

  datetime.weekday = rtc_read(RTC_REG_WEEKDAY);
  datetime.day_of_month = rtc_read(RTC_REG_DOM);
  datetime.month = rtc_read(RTC_REG_MONTH);
  datetime.year = rtc_read(RTC_REG_YEAR);
  datetime.second = rtc_read(RTC_REG_SEC);
  datetime.minute = rtc_read(RTC_REG_MIN);
  datetime.hour = rtc_read(RTC_REG_HOUR);

  return datetime;
}