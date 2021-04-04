#include "../include/kernel/irq.h"
#include "../include/kernel/idt.h"
#include "../include/kernel/asm.h"

/* IRQs */
extern void int_handler_32();
extern void int_handler_33();
extern void int_handler_34();
extern void int_handler_35();
extern void int_handler_36();
extern void int_handler_37();
extern void int_handler_38();
extern void int_handler_39();
extern void int_handler_40();
extern void int_handler_41();
extern void int_handler_42();
extern void int_handler_43();
extern void int_handler_44();
extern void int_handler_45();
extern void int_handler_46();
extern void int_handler_47();

void *irq_routines[16] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(int irq, void (*handler))
{
	irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq)
{
	irq_routines[irq] = 0;
}

void irq_install()
{
	/*
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x00);
	outb(0xA1, 0x00);
	*/

	idt_set_gate(32, (uint32_t)(uintptr_t)&int_handler_32, 0x8, 0x8e);
	idt_set_gate(33, (uint32_t)(uintptr_t)&int_handler_33, 0x8, 0x8e);
	idt_set_gate(34, (uint32_t)(uintptr_t)&int_handler_34, 0x8, 0x8e);
	idt_set_gate(35, (uint32_t)(uintptr_t)&int_handler_35, 0x8, 0x8e);
	idt_set_gate(36, (uint32_t)(uintptr_t)&int_handler_36, 0x8, 0x8e);
	idt_set_gate(37, (uint32_t)(uintptr_t)&int_handler_37, 0x8, 0x8e);
	idt_set_gate(38, (uint32_t)(uintptr_t)&int_handler_38, 0x8, 0x8e);
	idt_set_gate(39, (uint32_t)(uintptr_t)&int_handler_39, 0x8, 0x8e);
	idt_set_gate(40, (uint32_t)(uintptr_t)&int_handler_40, 0x8, 0x8e);
	idt_set_gate(41, (uint32_t)(uintptr_t)&int_handler_41, 0x8, 0x8e);
	idt_set_gate(42, (uint32_t)(uintptr_t)&int_handler_42, 0x8, 0x8e);
	idt_set_gate(43, (uint32_t)(uintptr_t)&int_handler_43, 0x8, 0x8e);
	idt_set_gate(44, (uint32_t)(uintptr_t)&int_handler_44, 0x8, 0x8e);
	idt_set_gate(45, (uint32_t)(uintptr_t)&int_handler_45, 0x8, 0x8e);
	idt_set_gate(46, (uint32_t)(uintptr_t)&int_handler_46, 0x8, 0x8e);
	idt_set_gate(47, (uint32_t)(uintptr_t)&int_handler_47, 0x8, 0x8e);

	idt_install();
}


/* This function gets called by a irq routine from assembler */
void irq_handler(int irq)
{
	void (*handler)();

	/* Lets see if we have a routine for that */
	handler = irq_routines[irq];

	if (handler) {
		handler();
	}

	// Send EOI
	outb(0x20, 0x20);
}
