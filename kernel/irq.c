#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/irq.h"
#include "../include/kernel/idt.h"
#include "../include/kernel/asm.h"
#include "../include/kernel/kernel.h"
#include "../config.h"

void *irq_routines[16] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(int irq, void (*handler))
{
	irq_routines[irq] = handler;

	#ifdef DEBUG_IRQ_INSTALL
		kdebug("[irq] installed handler for irq %d\r\n", irq);
	#endif
}

void irq_uninstall_handler(int irq)
{
	irq_routines[irq] = 0;
}

void irq_handler(uint8_t irq)
{
	#ifdef DEBUG_IRQ_PRINT
		kdebug("IRQ%d got fired!\r\n", (int)irq);
        #endif

	void (*handler)();

	/* Lets see if we have a routine for that */
	handler = irq_routines[irq];

	if (handler) {
		#ifdef DEBUG_IRQ_PRINT_ROUTINE
			kdebug("IRQ%d got fired!\r\n", (int)irq);
		#endif

		handler();
	}

	// Send EOI
	outb(0x20, 0x20);

	// Send EOI to slave pic
	if (irq >= 8) {
		outb(0xA0, 0x20);
	}
}

void irq_print_map()
{
	kdebug("IRQ Map\r\n");
	kdebug("| IRQ | Used |\r\n");

	for (int i=0; i<16; i++) {
		if (i >= 10) {
			kdebug("|  %d |   %s   |\r\n", i, (irq_routines[i] != 0) ? "x" : " ");
			continue;
		}

		kdebug("|  %d  |   %s   |\r\n", i, (irq_routines[i] != 0) ? "x" : " ");
	}
}
