#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/idt.h"
#include "../include/lib/print.h"
#include "../include/kernel/asm.h"
#include "../include/drivers/pic.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/kernel.h"

void idt_set_gate(unsigned int pos, uint32_t base,
   uint16_t selector, uint8_t type_attributes)
{
    idt[pos].base_lower = base & 0xffff;
    idt[pos].selector = selector;
    idt[pos].unused = 0x0;
    idt[pos].type_attributes = type_attributes;
    idt[pos].base_higher = (base >> 16) & 0xffff;
}

extern void int_handler_0();
extern void int_handler_1();
extern void int_handler_2();
extern void int_handler_3();
extern void int_handler_4();
extern void int_handler_5();
extern void int_handler_6();
extern void int_handler_7();
extern void int_handler_8();
extern void int_handler_9();
extern void int_handler_10();
extern void int_handler_11();
extern void int_handler_12();
extern void int_handler_13();
extern void int_handler_14();
extern void int_handler_15();
extern void int_handler_16();
extern void int_handler_17();
extern void int_handler_18();
extern void int_handler_19();
extern void int_handler_20();
extern void int_handler_21();
extern void int_handler_22();
extern void int_handler_23();
extern void int_handler_24();
extern void int_handler_25();
extern void int_handler_26();
extern void int_handler_27();
extern void int_handler_28();
extern void int_handler_29();
extern void int_handler_30();
extern void int_handler_31();
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
extern void int_handler_48();
extern void int_handler_49();
extern void int_handler_50();
extern void int_handler_51();
extern void int_handler_52();
extern void int_handler_53();
extern void int_handler_54();
extern void int_handler_55();
extern void int_handler_56();
extern void int_handler_57();
extern void int_handler_58();
extern void int_handler_59();
extern void int_handler_60();
extern void int_handler_61();
extern void int_handler_62();
extern void int_handler_63();
extern void int_handler_64();
extern void int_handler_65();
extern void int_handler_66();
extern void int_handler_67();
extern void int_handler_68();
extern void int_handler_69();
extern void int_handler_70();
extern void int_handler_71();
extern void int_handler_72();
extern void int_handler_73();
extern void int_handler_74();
extern void int_handler_75();
extern void int_handler_76();
extern void int_handler_77();
extern void int_handler_78();
extern void int_handler_79();
extern void int_handler_80();
extern void int_handler_81();
extern void int_handler_82();
extern void int_handler_83();
extern void int_handler_84();
extern void int_handler_85();
extern void int_handler_86();
extern void int_handler_87();
extern void int_handler_88();
extern void int_handler_89();
extern void int_handler_90();
extern void int_handler_91();
extern void int_handler_92();
extern void int_handler_93();
extern void int_handler_94();
extern void int_handler_95();
extern void int_handler_96();
extern void int_handler_97();
extern void int_handler_98();
extern void int_handler_99();

void idt_install()
{
	idt_desc.limit = IDT_ENTRIES * sizeof(struct idt_entry) - 1;
	idt_desc.base = (uint32_t) (uintptr_t) idt;

	/* Before we call this we must set up some isrs and irqs */

	/* These are isrs for kernel failures Note: Move them into seperate file! */
	idt_set_gate(0, (uint32_t)(uintptr_t)&int_handler_0, 0x8, 0x8e);
	idt_set_gate(1, (uint32_t)(uintptr_t)&int_handler_1, 0x8, 0x8e);
	idt_set_gate(2, (uint32_t)(uintptr_t)&int_handler_2, 0x8, 0x8e);
	idt_set_gate(3, (uint32_t)(uintptr_t)&int_handler_3, 0x8, 0x8e);
	idt_set_gate(4, (uint32_t)(uintptr_t)&int_handler_4, 0x8, 0x8e);
	idt_set_gate(5, (uint32_t)(uintptr_t)&int_handler_5, 0x8, 0x8e);
	idt_set_gate(6, (uint32_t)(uintptr_t)&int_handler_6, 0x8, 0x8e);
	idt_set_gate(7, (uint32_t)(uintptr_t)&int_handler_7, 0x8, 0x8e);
	idt_set_gate(8, (uint32_t)(uintptr_t)&int_handler_8, 0x8, 0x8e);
	idt_set_gate(9, (uint32_t)(uintptr_t)&int_handler_9, 0x8, 0x8e);
	idt_set_gate(10, (uint32_t)(uintptr_t)&int_handler_10, 0x8, 0x8e);
	idt_set_gate(11, (uint32_t)(uintptr_t)&int_handler_11, 0x8, 0x8e);
	idt_set_gate(12, (uint32_t)(uintptr_t)&int_handler_12, 0x8, 0x8e);
 	idt_set_gate(13, (uint32_t)(uintptr_t)&int_handler_13, 0x8, 0x8e);
	idt_set_gate(14, (uint32_t)(uintptr_t)&int_handler_14, 0x8, 0x8e);
	idt_set_gate(15, (uint32_t)(uintptr_t)&int_handler_15, 0x8, 0x8e);
	idt_set_gate(16, (uint32_t)(uintptr_t)&int_handler_16, 0x8, 0x8e);
	idt_set_gate(17, (uint32_t)(uintptr_t)&int_handler_17, 0x8, 0x8e);
	idt_set_gate(18, (uint32_t)(uintptr_t)&int_handler_18, 0x8, 0x8e);
	idt_set_gate(19, (uint32_t)(uintptr_t)&int_handler_19, 0x8, 0x8e);
	idt_set_gate(20, (uint32_t)(uintptr_t)&int_handler_20, 0x8, 0x8e);
	idt_set_gate(21, (uint32_t)(uintptr_t)&int_handler_21, 0x8, 0x8e);
	idt_set_gate(22, (uint32_t)(uintptr_t)&int_handler_22, 0x8, 0x8e);
	idt_set_gate(23, (uint32_t)(uintptr_t)&int_handler_23, 0x8, 0x8e);
	idt_set_gate(24, (uint32_t)(uintptr_t)&int_handler_24, 0x8, 0x8e);
	idt_set_gate(25, (uint32_t)(uintptr_t)&int_handler_25, 0x8, 0x8e);
	idt_set_gate(26, (uint32_t)(uintptr_t)&int_handler_26, 0x8, 0x8e);
	idt_set_gate(27, (uint32_t)(uintptr_t)&int_handler_27, 0x8, 0x8e);
	idt_set_gate(28, (uint32_t)(uintptr_t)&int_handler_28, 0x8, 0x8e);
	idt_set_gate(29, (uint32_t)(uintptr_t)&int_handler_29, 0x8, 0x8e);
	idt_set_gate(30, (uint32_t)(uintptr_t)&int_handler_30, 0x8, 0x8e);
	idt_set_gate(31, (uint32_t)(uintptr_t)&int_handler_31, 0x8, 0x8e);
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

	idt_set_gate(48, (uint32_t)(uintptr_t)&int_handler_48, 0x8, 0x8e);
    	idt_set_gate(49, (uint32_t)(uintptr_t)&int_handler_49, 0x8, 0x8e);
    	idt_set_gate(50, (uint32_t)(uintptr_t)&int_handler_50, 0x8, 0x8e);
	idt_set_gate(51, (uint32_t)(uintptr_t)&int_handler_51, 0x8, 0x8e);
	idt_set_gate(52, (uint32_t)(uintptr_t)&int_handler_52, 0x8, 0x8e);
	idt_set_gate(53, (uint32_t)(uintptr_t)&int_handler_53, 0x8, 0x8e);
	idt_set_gate(54, (uint32_t)(uintptr_t)&int_handler_54, 0x8, 0x8e);
    	idt_set_gate(55, (uint32_t)(uintptr_t)&int_handler_55, 0x8, 0x8e);
    	idt_set_gate(56, (uint32_t)(uintptr_t)&int_handler_56, 0x8, 0x8e);
    	idt_set_gate(57, (uint32_t)(uintptr_t)&int_handler_57, 0x8, 0x8e);
    idt_set_gate(58, (uint32_t)(uintptr_t)&int_handler_58, 0x8, 0x8e);
    idt_set_gate(59, (uint32_t)(uintptr_t)&int_handler_59, 0x8, 0x8e);
    idt_set_gate(60, (uint32_t)(uintptr_t)&int_handler_60, 0x8, 0x8e);
    idt_set_gate(61, (uint32_t)(uintptr_t)&int_handler_61, 0x8, 0x8e);
    idt_set_gate(62, (uint32_t)(uintptr_t)&int_handler_62, 0x8, 0x8e);
    idt_set_gate(63, (uint32_t)(uintptr_t)&int_handler_63, 0x8, 0x8e);
    idt_set_gate(64, (uint32_t)(uintptr_t)&int_handler_64, 0x8, 0x8e);
    idt_set_gate(65, (uint32_t)(uintptr_t)&int_handler_65, 0x8, 0x8e);
    idt_set_gate(66, (uint32_t)(uintptr_t)&int_handler_66, 0x8, 0x8e);
    idt_set_gate(67, (uint32_t)(uintptr_t)&int_handler_67, 0x8, 0x8e);
    idt_set_gate(68, (uint32_t)(uintptr_t)&int_handler_68, 0x8, 0x8e);
    idt_set_gate(69, (uint32_t)(uintptr_t)&int_handler_69, 0x8, 0x8e);
    idt_set_gate(70, (uint32_t)(uintptr_t)&int_handler_70, 0x8, 0x8e);
    idt_set_gate(71, (uint32_t)(uintptr_t)&int_handler_71, 0x8, 0x8e);
    idt_set_gate(72, (uint32_t)(uintptr_t)&int_handler_72, 0x8, 0x8e);
    idt_set_gate(73, (uint32_t)(uintptr_t)&int_handler_73, 0x8, 0x8e);
    idt_set_gate(74, (uint32_t)(uintptr_t)&int_handler_74, 0x8, 0x8e);
    idt_set_gate(75, (uint32_t)(uintptr_t)&int_handler_75, 0x8, 0x8e);
    idt_set_gate(76, (uint32_t)(uintptr_t)&int_handler_76, 0x8, 0x8e);
    idt_set_gate(77, (uint32_t)(uintptr_t)&int_handler_77, 0x8, 0x8e);
    idt_set_gate(78, (uint32_t)(uintptr_t)&int_handler_78, 0x8, 0x8e);
    idt_set_gate(79, (uint32_t)(uintptr_t)&int_handler_79, 0x8, 0x8e);
    idt_set_gate(80, (uint32_t)(uintptr_t)&int_handler_80, 0x8, 0x8e);
    idt_set_gate(81, (uint32_t)(uintptr_t)&int_handler_81, 0x8, 0x8e);
    idt_set_gate(82, (uint32_t)(uintptr_t)&int_handler_82, 0x8, 0x8e);
    idt_set_gate(83, (uint32_t)(uintptr_t)&int_handler_83, 0x8, 0x8e);
    idt_set_gate(84, (uint32_t)(uintptr_t)&int_handler_84, 0x8, 0x8e);
    idt_set_gate(85, (uint32_t)(uintptr_t)&int_handler_85, 0x8, 0x8e);
    idt_set_gate(86, (uint32_t)(uintptr_t)&int_handler_86, 0x8, 0x8e);
    idt_set_gate(87, (uint32_t)(uintptr_t)&int_handler_87, 0x8, 0x8e);
    idt_set_gate(88, (uint32_t)(uintptr_t)&int_handler_88, 0x8, 0x8e);
    idt_set_gate(89, (uint32_t)(uintptr_t)&int_handler_89, 0x8, 0x8e);
    idt_set_gate(90, (uint32_t)(uintptr_t)&int_handler_90, 0x8, 0x8e);
    idt_set_gate(91, (uint32_t)(uintptr_t)&int_handler_91, 0x8, 0x8e);
    idt_set_gate(92, (uint32_t)(uintptr_t)&int_handler_92, 0x8, 0x8e);
    idt_set_gate(93, (uint32_t)(uintptr_t)&int_handler_93, 0x8, 0x8e);
    idt_set_gate(94, (uint32_t)(uintptr_t)&int_handler_94, 0x8, 0x8e);
    idt_set_gate(95, (uint32_t)(uintptr_t)&int_handler_95, 0x8, 0x8e);
    idt_set_gate(96, (uint32_t)(uintptr_t)&int_handler_96, 0x8, 0x8e);
    idt_set_gate(97, (uint32_t)(uintptr_t)&int_handler_97, 0x8, 0x8e);
    idt_set_gate(98, (uint32_t)(uintptr_t)&int_handler_98, 0x8, 0x8e);
    for (int i = 99; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, (uint32_t)(uintptr_t)&int_handler_99,
            0x8, 0x8e);
    }


	// Master
	outb(PIC_MASTER_COMMAND_PORT, 0x11);

	outb(PIC_MASTER_DATA_PORT, 0x20); // IRQ0
	outb(PIC_MASTER_DATA_PORT, 0x04); // IRQ2
	outb(PIC_MASTER_DATA_PORT, 0x01);

	// Slave
	outb(PIC_SLAVE_COMMAND_PORT, 0x11);

	outb(PIC_SLAVE_DATA_PORT, 0x28); // IRQ8
	outb(PIC_SLAVE_DATA_PORT, 0x04); // IRQ2
	outb(PIC_SLAVE_DATA_PORT, 0x01);

	// Demask
	outb(PIC_MASTER_DATA_PORT, 0x0);
	outb(PIC_SLAVE_DATA_PORT, 0x0);

    	asm volatile("lidt %0" :: "m"(idt_desc));
    	asm volatile("xchg %bx, %bx");
    	asm volatile("sti");
}

void int_handler(struct cpu_state* state)
{
	// Here we receive the cpu state
	if (state->intr < 20) {
		// Kernel Exception
		kpanic(state->intr);
	} else if (state->intr > 31){
		// IRQ
		irq_handler(state->intr - 32);
	}
}
