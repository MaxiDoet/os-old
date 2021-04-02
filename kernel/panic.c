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

	int debug_port;

        // Init serial debug
  	#ifdef DEBUG_COM1
                debug_port = 0x3F8;
        #elif DEBUG_COM2
                debug_port = 0x2F8;
        #elif DEBUG_COM3
                debug_port = 0x3E8;
        #elif DEBUG_COM4
                debug_port = 0x2E8;
        #else
                debug_port = 0x3F8;
        #endif

        serial_init(debug_port);
	kdebug(debug_port, "Kernel Panic! ");

	switch(exception) {
		case 0x00:
			kdebug(debug_port, "Division by Null");
			break;
		case 0x01:
			kdebug(debug_port, "Debug");
			break;
		case 0x02:
			kdebug(debug_port, "NMI");
			break;
		case 0x03:
			kdebug(debug_port, "Breakpoint");
			break;
		case 0x04:
			kdebug(debug_port, "Overflow");
			break;
		case 0x05:
			kdebug(debug_port, "Bounds Check");
			break;
		case 0x06:
			kdebug(debug_port, "Invalid Opcode");
			break;
		case 0x07:
			kdebug(debug_port, "NPX Not Avaiable");
			break;
		case 0x08:
			kdebug(debug_port, "Double Fault");
			break;
		case 0x09:
			kdebug(debug_port, "NPX Segment Overrun");
			break;
		case 0x10:
			kdebug(debug_port, "Invalid Task State Segment(TSS)");
			break;
		case 0x11:
			kdebug(debug_port, "Segment Not Preset");
			break;
		case 0x12:
			kdebug(debug_port, "Stack Fault");
			break;
		case 0x13:
			kdebug(debug_port, "General Protection");
			break;
		case 0x14:
			kdebug(debug_port, "Page Fault");
			break;
		case 0x15:
			// Note: Add some more details here
			kdebug(debug_port, "Intel Reserved");
			break;
		case 0x16:
			kdebug(debug_port, "Floating Point");
			break;
		case 0x17:
			kdebug(debug_port, "Alignment Check");
			break;
		case 0x18:
			kdebug(debug_port, "Machine Check");
			break;
		case 0x19:
			kdebug(debug_port, "SIMD Floating Point");
			break;
		default:
			kdebug(debug_port, "Unknown Exception");
			break;
	}

	for(;;);
}

