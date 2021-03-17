#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/vga.h"
#include "../include/print.h"
#include "../include/cmos.h"
#include "../include/pci.h"
#include "../include/serial.h"

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
		for (slot=0; slot <= 32; slot++) {
			uint16_t vendorId = pci_read_word(bus, slot, 0, 0x00);
			uint16_t deviceId = pci_read_word(bus, slot, 0, 0x02);
			uint8_t classCode = pci_read_word(bus, slot, 0, 0x0B);
			uint8_t subclass = pci_read_word(bus, slot, 0, 0x0A);

			if (vendorId == 0xFFFF || vendorId == 0x00FF) {
				continue;
			}

			prints(" PCI Device: ");
			prints("Bus: ");
			printi(bus);
			prints(" Slot: ");
			printi(slot);
			prints(" VendorID: ");
			printh16(vendorId);
			prints(" DeviceID: ");
			printh16(deviceId);
			prints("\n  ");
			/*
			prints(" Class: ");
			printh8(classCode);
			prints(" Subclass: ");
			printh8(subclass);
			*/

			// Identify devices

			switch(vendorId) {

				case 0x1FA4:
					switch(deviceId) {
						// Virtio Filesystem
						case 0x1049:
							prints("Virtio Filesystem");
							break;

						default:
							prints("Unknown Device");
					}
					break;

				case 0x10EC:
					switch(deviceId) {
						case 0x8029:
							prints("RTL-8029");
							break;
						default:
							prints("Unknown Device");
					}
					break;

				case 0x8086:
					switch(deviceId) {
						case 0x7010:
							prints("82371SB PIIX3 IDE [Natoma/Triton II]");
							break;
						case 0x7000:
							prints("82371SB PIIX3 ISA [Natoma/Triton II]");
							break;
						case 0x1237:
							prints("440FX - 82441FX PMC [Natoma] Chipset");
							break;
						default:
							prints("Unknown Device");
					}
					break;

				default:
					prints("Unknown Device");
			}

			prints("\n\n");

			prints("Serial: COM1\n");

			serial_write('H');
			serial_write('e');
			serial_write('l');
			serial_write('l');
                        serial_write('o');

			while(1) {
				printc((char)serial_read());
			}

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
