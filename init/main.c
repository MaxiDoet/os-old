#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../config.h"
#include "../include/kernel/kernel.h"
#include "../include/drivers/serial.h"
#include "../include/drivers/vgacon.h"
#include "../include/drivers/vga-color.h"
#include "../include/lib/print.h"
#include "../include/lib/convert.h"
#include "../include/kernel/gdt.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/idt.h"
#include "../include/kernel/multiboot.h"
#include "../include/kernel/kernel.h"
#include "../include/drivers/pci.h"
#include "../include/kernel/asm.h"

#include "../bin/shell/shell.h"
#include "../bin/desktop/desktop.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

int timer_ticks=0;

void timer_phase(int hz)
{
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    outb(0x43, 0x36);             /* Set our command byte 0x36 */
    outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

void timer_irq_handler() {
	timer_ticks++;

	kdebug(0x3F8, "Tick");

	if(timer_ticks % 18 == 0) {
		kdebug(0x3F8, "One second passed");
	}
}

void kmain(unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;
	mbi = (multiboot_info_t *) addr;

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

	kdebug(debug_port, "Multiboot Magic: ");

	if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
		kdebug(debug_port, "Correct!\r\n");
	} else {
		kdebug(debug_port, "Not correct!\r\n");
	}

	void *fb = (void *) (unsigned long) mbi->framebuffer_addr;

	kdebug(debug_port, "GDT init\r\n");
	gdt_setup();
	kdebug(debug_port, "IDT init\r\n");
	irq_install();

	irq_install_handler(0, timer_irq_handler);
	timer_phase(100);


	/* Scan for pci devices and enable drivers */
	kdebug(debug_port, "PCI Scan\r\n");

	kdebug(debug_port, "| VendorID | DeviceID | Class | Subclass |\n\r");
	kdebug(debug_port, "|----------------------------------------|\n\r");

	int bus, slot;
	for (bus=0; bus < 256; bus++) {
		for (slot=0; slot < 32; slot++) {
			uint16_t vendorId = pci_get_vendor_id(bus, slot, 0);
			uint16_t deviceId = pci_get_device_id(bus, slot, 0);
			uint8_t classCode = pci_get_class_code(bus, slot, 0);
			uint8_t subclass = pci_get_subclass(bus, slot, 0);

			if (vendorId == 0xFFFF || vendorId == 0x00FF) {
				continue;
			}

			/*
			switch(classCode) {
				case 0x00:
					kdebug(debug_port,  " Unclassified PCI Device ");
					break;
				case 0x01:
					kdebug(debug_port,  " Mass Storage Controller ");
					break;
				case 0x02:
					kdebug(debug_port,  " Network Controller ");
					break;
				case 0x03:
					kdebug(debug_port,  " Display Controller ");
					break;
				case 0x04:
					kdebug(debug_port,  " Multimedia Controller ");
					break;
				case 0x05:
					kdebug(debug_port,  " Memory Controller ");
					break;
				case 0x06:
					kdebug(debug_port,  " Bridge Device ");
					break;
				case 0x07:
					kdebug(debug_port,  " Simple Communication Controller ");
					break;
				case 0x08:
					kdebug(debug_port,  " Base System Peripheral ");
					break;
				case 0x09:
					kdebug(debug_port,  " Input Device Controller ");
					break;
				case 0x0A:
					kdebug(debug_port,  " Docking Station ");
					break;
				case 0x0B:
					kdebug(debug_port,  " Processor ");
					break;
				case 0x0C:
					kdebug(debug_port,  " Serial Bus Controller ");
					break;
				case 0x0D:
					kdebug(debug_port,  " Wireless Controller ");
					break;
				case 0x0E:
					kdebug(debug_port,  " Intelligent Controller ");
					break;
				case 0x0F:
					kdebug(debug_port,  " Satellite Communication Controller ");
					break;
				case 0x10:
					kdebug(debug_port,  " Encryption Controller ");
					break;
				case 0x11:
					kdebug(debug_port,  " Signal Processing Controller ");
					break;
				case 0x12:
					kdebug(debug_port,  " Processing Accelerator ");
					break;
				case 0x13:
					kdebug(debug_port,  " Non-Essential Instrumentation ");
					break;
				case 0x14:
					kdebug(debug_port,  " Reserved ");
					break;
				case 0x40:
					kdebug(debug_port,  " Co-Processor ");
					break;
				case 0x41:
					kdebug(debug_port,  " Reserved ");
					break;
				case 0xFF:
					kdebug(debug_port,  " Unassigned Class ");
					break;
				default:
					kdebug(debug_port,  " Unassigned Class ");
					break;
			}
			*/
			kdebug(debug_port,  "| %x ", vendorId);
			kdebug(debug_port,  "      %x\r\n", deviceId);

			// Identify devices
			/*
			switch(vendorId) {

				case 0x1FA4:
					switch(deviceId) {
						// Virtio Filesystem
						case 0x1049:
							kdebug(debug_port,  "Virtio Filesystem\r\n");
							break;

						default:
							kdebug(debug_port,  "Unknown Device\r\n");
					}
					break;

				case 0x10EC:
					switch(deviceId) {
						case 0x8029:
							kdebug(debug_port,  "RTL-8029\r\n");
							break;
						default:
							kdebug(debug_port,  "Unknown Device\r\n");
					}
					break;

				case 0x8086:
					switch(deviceId) {
						case 0x7010:
							kdebug(debug_port,  "82371SB PIIX3 IDE [Natoma/Triton II]\r\n");
							break;
						case 0x7000:
							kdebug(debug_port,  "82371SB PIIX3 ISA [Natoma/Triton II]\r\n");
							break;
						case 0x1237:
							kdebug(debug_port,  "440FX - 82441FX PMC [Natoma] Chipset\r\n");
							break;
						case 0x100E:
							kdebug(debug_port,  "82540EM Gigabit Ethernet Controller\r\n");
							break;
						default:
							kdebug(debug_port,  "Unknown Device\r\n");
					}
					break;

				case 0x1013:
                                        switch(deviceId) {
                                                case 0x00b8:
                                                        kdebug(debug_port,  "Cirrus Logic GD5446\r\n");
                                                        break;
                                                default:
                                                  	kdebug(debug_port,  "Unknown Device\r\n");
                                        }
                                        break;

				default:
					kdebug(debug_port,  "Unknown Device\r\n");
			}
			*/
		}
	}

	kdebug(debug_port,  "|----------------------------------------|\n\r");

	kdebug(debug_port, "Test:\r\nNumber: %d, Hex: %x", 24, 0x024a);

	timer_phase(100);

	#ifdef DESKTOP
		desktop_init(fb, mbi);
	#endif
}

void keyboard_irq_handler()
{
	prints("Keyboard IRQ");
}
