#ifndef _rtc_h
#define _rtc_h

#include "asm.h"

size_t rtc_read(int address)
{
	outb_p(0x80 | address, 0x70);
	return inb_p(0x71);
}

#endif
