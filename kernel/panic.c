#include <stddef.h>
#include <stdint.h>
#include "../include/drivers/serial.h"
#include "../include/kernel/kernel.h"
#include "../config.h"

void kpanic(int exception)
{
        serial_init(DEBUG_PORT);

	switch(exception) {
		case 0x00:
			kdebug("[\e[0;31mpanic\e[0;37m] Division by Null");
			break;
		case 0x01:
			kdebug("[\e[0;31mpanic\e[0;37m] Debug");
			break;
		case 0x02:
			kdebug("[\e[0;31mpanic\e[0;37m] NMI");
			break;
		case 0x03:
			kdebug("[\e[0;31mpanic\e[0;37m] Breakpoint");
			break;
		case 0x04:
			kdebug("[\e[0;31mpanic\e[0;37m] Overflow");
			break;
		case 0x05:
			kdebug("[\e[0;31mpanic\e[0;37m] Bounds Check");
			break;
		case 0x06:
			kdebug("[\e[0;31mpanic\e[0;37m] Invalid Opcode");
			break;
		case 0x07:
			kdebug("[\e[0;31mpanic\e[0;37m] NPX Not Avaiable");
			break;
		case 0x08:
			kdebug("[\e[0;31mpanic\e[0;37m] Double Fault");
			break;
		case 0x09:
			kdebug("[\e[0;31mpanic\e[0;37m] NPX Segment Overrun");
			break;
		case 0x10:
			kdebug("[\e[0;31mpanic\e[0;37m] Invalid Task State Segment(TSS)");
			break;
		case 0x11:
			kdebug("[\e[0;31mpanic\e[0;37m] Segment Not Preset");
			break;
		case 0x12:
			kdebug("[\e[0;31mpanic\e[0;37m] Stack Fault");
			break;
		case 0x13:
			kdebug("[\e[0;31mpanic\e[0;37m] General Protection");
			break;
		case 0x14:
			kdebug("[\e[0;31mpanic\e[0;37m] Page Fault");
			break;
		case 0x15:
			// Note: Add some more details here
			kdebug("[\e[0;31mpanic\e[0;37m] Intel Reserved");
			break;
		case 0x16:
			kdebug("[\e[0;31mpanic\e[0;37m] Floating Point");
			break;
		case 0x17:
			kdebug("[\e[0;31mpanic\e[0;37m] Alignment Check");
			break;
		case 0x18:
			kdebug("[\e[0;31mpanic\e[0;37m] Machine Check");
			break;
		case 0x19:
			kdebug("[\e[0;31mpanic\e[0;37m] SIMD Floating Point");
			break;
		default:
			kdebug("[\e[0;31mpanic\e[0;37m] Unknown Exception");
			break;
	}

	for(;;);
}

