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
#include "../include/drivers/sb16.h"
#include "../include/drivers/pit.h"
#include "../include/kernel/asm.h"

#include "../bin/shell/shell.h"
#include "../bin/desktop/desktop.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

void kmain(unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;
	mbi = (multiboot_info_t *) addr;

	int debug_port;

	// Init serial debug

	serial_init(DEBUG_PORT);

	kdebug("Multiboot Magic: ");

	if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
		kdebug("Correct!\r\n");
	} else {
		kdebug("Not correct!\r\n");
	}

	void *fb = (void *) (unsigned long) mbi->framebuffer_addr;

	beep(1000);

	kdebug("-------- Stage 1 --------\r\n");

	kdebug("GDT init\r\n");
	gdt_setup();
	kdebug("IDT init\r\n");
	irq_install();

	kdebug("-------- Stage 2 --------\r\n");

	if(sb16_probe() == 0) {
		sb16_init();
	}

	/* Scan for pci devices and enable drivers */
	kdebug("PCI Scan\r\n");

	kdebug("| VendorID | DeviceID | Class | Subclass |\n\r");
	kdebug("|----------------------------------------|\n\r");

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
			kdebug("| %x ", vendorId);
			kdebug("      %x\r\n", deviceId);

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

	kdebug("|----------------------------------------|\n\r");

	#ifdef DESKTOP
		desktop_init(fb, mbi);
	#endif
}
