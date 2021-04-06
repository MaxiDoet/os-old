#include <stddef.h>
#include <stdint.h>
#include "../include/drivers/serial.h"
#include "../include/kernel/kernel.h"
#include "../config.h"

void kpanic(int exception)
{
	/*
		0: Division by null
	*/

        serial_init(DEBUG_PORT);
	kdebug("Kernel Panic! ");

	switch(exception) {
		case 0x00:
			kdebug("Division by Null");
			break;
		case 0x01:
			kdebug("Debug");
			break;
		case 0x02:
			kdebug("NMI");
			break;
		case 0x03:
			kdebug("Breakpoint");
			break;
		case 0x04:
			kdebug("Overflow");
			break;
		case 0x05:
			kdebug("Bounds Check");
			break;
		case 0x06:
			kdebug("Invalid Opcode");
			break;
		case 0x07:
			kdebug("NPX Not Avaiable");
			break;
		case 0x08:
			kdebug("Double Fault");
			break;
		case 0x09:
			kdebug("NPX Segment Overrun");
			break;
		case 0x10:
			kdebug("Invalid Task State Segment(TSS)");
			break;
		case 0x11:
			kdebug("Segment Not Preset");
			break;
		case 0x12:
			kdebug("Stack Fault");
			break;
		case 0x13:
			kdebug("General Protection");
			break;
		case 0x14:
			kdebug("Page Fault");
			break;
		case 0x15:
			// Note: Add some more details here
			kdebug("Intel Reserved");
			break;
		case 0x16:
			kdebug("Floating Point");
			break;
		case 0x17:
			kdebug("Alignment Check");
			break;
		case 0x18:
			kdebug("Machine Check");
			break;
		case 0x19:
			kdebug("SIMD Floating Point");
			break;

		case 0x32:
			kdebug("Timer");
			break;
		default:
			kdebug("Unknown Exception");
			break;
	}

	for(;;);
}

