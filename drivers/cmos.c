#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/cmos.h"
#include "../include/asm.h"

uint8_t cmos_read(uint8_t offset) {
  uint8_t tmp = inb(CMOS_PORT_ADDRESS);
  outb(CMOS_PORT_ADDRESS, (tmp & 0x80) | (offset & 0x7F));
  return inb(CMOS_PORT_DATA);
}

void cmos_write(uint8_t offset,uint8_t val) {
  uint8_t tmp = inb(CMOS_PORT_ADDRESS);
  outb(CMOS_PORT_ADDRESS, (tmp & 0x80) | (offset & 0x7F));
  outb(CMOS_PORT_DATA,val);
}

uint8_t cmos_rtc_decode(uint8_t bcd)
{
	return (bcd & 0xF) + ((bcd / 16) * 10);
}
