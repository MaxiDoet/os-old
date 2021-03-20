#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/vga.h"
#include "../include/print.h"
#include "../include/cmos.h"
#include "../include/pci.h"
#include "../include/serial.h"
#include "../include/bga.h"

void kernel_main(void) {

	vga_init(VGA_TEXT_MODE);

	vga_clear(VGA_TEXT_COLOR_BLACK);

	vga_set_color(VGA_TEXT_COLOR_GREEN, VGA_TEXT_COLOR_BLACK);

	prints("Welcome\n\n");

	// CMOS Dump
	prints("CMOS Dump:\n");

	prints(" RTC: ");

	printi(cmos_rtc_decode(cmos_read(4)));
	prints(":");

	/* If minute is less than 10 print leading zero */
	if (cmos_rtc_decode(cmos_read(2)) < 10) {
		prints("0");
	}

	printi(cmos_rtc_decode(cmos_read(2)));
	prints(":");
	printi(cmos_rtc_decode(cmos_read(0)));

	prints(" ");
	printi(cmos_rtc_decode(cmos_read(7)));
	prints("/");
	printi(cmos_rtc_decode(cmos_read(8)));
	prints("/");
	printi(cmos_rtc_decode(cmos_read(9)));
	prints("\n");

	// Brute-Force pci scan

	prints("\nPCI Devices:\n");

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

			switch(classCode) {
				case 0x00:
					prints(" Unclassified PCI Device ");
					break;
				case 0x01:
					prints(" Mass Storage Controller ");
					break;
				case 0x02:
					prints(" Network Controller ");
					break;
				case 0x03:
					prints(" Display Controller ");
					break;
				case 0x04:
					prints(" Multimedia Controller ");
					break;
				case 0x05:
					prints(" Memory Controller ");
					break;
				case 0x06:
					prints(" Bridge Device ");
					break;
				case 0x07:
					prints(" Simple Communication Controller ");
					break;
				case 0x08:
					prints(" Base System Peripheral ");
					break;
				case 0x09:
					prints(" Input Device Controller ");
					break;
				case 0x0A:
					prints(" Docking Station ");
					break;
				case 0x0B:
					prints(" Processor ");
					break;
				case 0x0C:
					prints(" Serial Bus Controller ");
					break;
				case 0x0D:
					prints(" Wireless Controller ");
					break;
				case 0x0E:
					prints(" Intelligent Controller ");
					break;
				case 0x0F:
					prints(" Satellite Communication Controller ");
					break;
				case 0x10:
					prints(" Encryption Controller ");
					break;
				case 0x11:
					prints(" Signal Processing Controller ");
					break;
				case 0x12:
					prints(" Processing Accelerator ");
					break;
				case 0x13:
					prints(" Non-Essential Instrumentation ");
					break;
				case 0x14:
					prints(" Reserved ");
					break;
				case 0x40:
					prints(" Co-Processor ");
					break;
				case 0x41:
					prints(" Reserved ");
					break;
				case 0xFF:
					prints(" Unassigned Class ");
					break;
				default:
					prints(" Unassigned Class ");
					break;
			}

			prints(" VendorID: ");
			printh16(vendorId);
			prints(" DeviceID: ");
			printh16(deviceId);
			prints(" Class: ");
			printh8(classCode);
			prints(" Subclass: ");
			printh8(subclass);
			prints(" \n");

			// Identify devices

			switch(vendorId) {

				case 0x1FA4:
					switch(deviceId) {
						// Virtio Filesystem
						case 0x1049:
							prints("Virtio Filesystem\n");
							break;

						default:
							prints("Unknown Device\n");
					}
					break;

				case 0x10EC:
					switch(deviceId) {
						case 0x8029:
							prints("RTL-8029\n");
							break;
						default:
							prints("Unknown Device\n");
					}
					break;

				case 0x8086:
					switch(deviceId) {
						case 0x7010:
							prints("82371SB PIIX3 IDE [Natoma/Triton II]\n");
							break;
						case 0x7000:
							prints("82371SB PIIX3 ISA [Natoma/Triton II]\n");
							break;
						case 0x1237:
							prints("440FX - 82441FX PMC [Natoma] Chipset\n");
							break;
						case 0x100E:
							prints("82540EM Gigabit Ethernet Controller\n");
							break;
						default:
							prints("Unknown Device\n");
					}
					break;

				case 0x1013:
                                        switch(deviceId) {
                                                case 0x00b8:
                                                        prints("Cirrus Logic GD5446\n");
                                                        break;
                                                default:
                                                  	prints("Unknown Device\n");
                                        }
                                        break;

				default:
					prints("Unknown Device\n");
			}

			bga_set_video_mode(800, 600, VBE_DISPI_BPP_24);

			/*
			// Prime number test
			int n = 100000;
			int i = 3, count, c;

  			for (count = 2; count <= n;)
  			{
    				for (c = 2; c <= i - 1; c++)
    				{
      					if (i%c == 0)
						break;
    				}
    				if (c == i)
    				{
					prints("Found prime number: ");
      					printi(i);
					prints("\n");
      					count++;
    				}
    				i++;
  			}
			*/
		}
	}
}
