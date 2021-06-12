#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/pci.h"
#include "../include/kernel/io.h"
#include "../include/kernel/kernel.h"
#include "../libc/include/mm.h"

#include "../include/drivers/rtl8139.h"

uint32_t pci_read_dword(uint16_t bus, uint16_t device, uint16_t func, uint32_t offset)
{
        uint32_t address;
	uint32_t tmp;

        uint32_t lbus = (uint32_t) bus;
        uint32_t ldevice = (uint32_t) device;
        uint32_t lfunc = (uint32_t) func;

        address = (uint32_t)((lbus << 16) | (ldevice << 11) | (lfunc << 8) | (offset & 0xFC) | (uint32_t) 0x80000000);
        outl(PCI_CONFIG_ADDRESS, address); // Select address
	tmp = inl(PCI_CONFIG_DATA);
        return tmp;
}

uint16_t pci_read_word(uint16_t bus, uint16_t device, uint16_t func, uint16_t offset)
{
	uint32_t tmp;
	tmp = pci_read_dword(bus, device, func, offset);

	return (tmp >> ((offset & 3) * 8)) & 0xffff;
}

void pci_write_dword(uint16_t bus, uint16_t device, uint16_t func, uint32_t offset, uint32_t value)
{
        uint32_t address;

        uint32_t lbus = (uint32_t) bus;
        uint32_t ldevice = (uint32_t) device;
        uint32_t lfunc = (uint32_t) func;

        address = (uint32_t)((lbus << 16) | (ldevice << 11) | (lfunc << 8) | (offset & 0xFC) | (uint32_t) 0x80000000);
        outl(PCI_CONFIG_ADDRESS, address); // Select address
        outl(PCI_CONFIG_DATA, value);
}

void pci_write_word(uint16_t bus, uint16_t device, uint16_t func, uint32_t offset, uint16_t value)
{
        uint32_t address;

        uint32_t lbus = (uint32_t) bus;
        uint32_t ldevice = (uint32_t) device;
        uint32_t lfunc = (uint32_t) func;

        address = (uint32_t)((lbus << 16) | (ldevice << 11) | (lfunc << 8) | (offset & 0xFC) | (uint32_t) 0x80000000);
        outl(PCI_CONFIG_ADDRESS, address); // Select address
        outl(PCI_CONFIG_DATA, (uint16_t) value);
}

pci_dev_descriptor pci_get_dev_descriptor(uint16_t bus, uint16_t device, uint16_t func)
{
	pci_dev_descriptor dev;

	dev.bus = bus;
	dev.device = bus;
	dev.function = func;

	dev.vendor_id = pci_read_dword(bus, device, func, 0x00);
	dev.device_id = pci_read_dword(bus, device, func, 0x00) >> 16;

	dev.class_id = pci_read_dword(bus, device, func, 0x08) >> 24 & 0xFF;
	dev.subclass_id = pci_read_dword(bus, device, func, 0x08) >> 16 & 0xFF;
	dev.prog_if = pci_read_dword(bus, device, func, 0x08) >> 8 & 0xFF00;
	dev.revision_id = pci_read_dword(bus, device, func, 0x08) & 0xFF;

	dev.command = pci_read_dword(bus, device, func, 0x04);
	dev.irq = pci_read_word(bus, device, func, 0x3C);

	return dev;
}

pci_bar_descriptor pci_get_bar_descriptor(uint16_t bus, uint16_t device, uint16_t func, int barNum)
{
	pci_bar_descriptor bar;

	uint32_t headertype = pci_read_dword(bus, device, func, 0x0E) & 0x7F;

	int maxBars = 6 - (4*headertype);

	if (barNum >= maxBars) return bar;

	uint32_t bar_value = pci_read_dword(bus, device, func, 0x10 + 4*barNum);
	bar.type = (bar_value & 0x1) ? PCI_BAR_LAYOUT_IO : PCI_BAR_LAYOUT_MEMORYMAPPING;

	if (bar.type == PCI_BAR_LAYOUT_MEMORYMAPPING) {
		switch((bar_value >> 1) & 0x03) {
			case 0:
			case 1:
			case 2:
				// Not implemented yet!
				break;
		}
	} else {
		bar.address = (uint8_t*)(bar_value & ~0x3);
	}

	return bar;
}

void pci_scan()
{
	/* This performs a brute force scan */

	int bus_num;
	int slot_num;
	int function_num;
	int bar_num;

	for (bus_num=0; bus_num < 10; bus_num++) {
		for (slot_num=0; slot_num < 32; slot_num++) {
			for (function_num=0; function_num < 6; function_num++) {
				pci_dev_descriptor dev = pci_get_dev_descriptor(bus_num, slot_num, function_num);

				if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF) continue;

				for(bar_num = 0; bar_num < 6; bar_num++) {
					pci_bar_descriptor bar = pci_get_bar_descriptor(bus_num, slot_num, function_num, bar_num);

					if (bar.address && (bar.type == PCI_BAR_LAYOUT_IO)) {
						dev.io_base = (uint32_t) bar.address;
					}

					if (bar.address && (bar.type == PCI_BAR_LAYOUT_MEMORYMAPPING)) {
						dev.mem_base = (uint32_t) bar.address;
					}

				}

				kdebug("[pci] Bus: %d Slot: %d Func: %d Subclass: %x Prog IF: %x Vendor: %x Device: %x ", bus_num, slot_num, function_num, dev.subclass_id, dev.prog_if, dev.vendor_id, dev.device_id);

				switch(dev.class_id) {
					case 0x00:
						dev.description = "Unclassified PCI Device";
						break;
					case 0x01:
						switch(dev.subclass_id) {
							case 0x00:
								dev.description = "SCSI Bus Controller";
								break;
							case 0x01:
								dev.description = "IDE Controller";
								break;
							case 0x02:
								dev.description = "Floppy Disk Controller";
								break;
							case 0x04:
								dev.description = "RAID Controller";
								break;
							case 0x05:
								dev.description = "ATA Controller";
								break;
							case 0x06:
								dev.description = "Serial ATA Controller";
								break;
							case 0x07:
								dev.description = "Serial Attached SCSI Controller";
								break;
							case 0x08:
								dev.description = "NVM Controller";
								break;
							default:
								dev.description = "Mass Storage Controller";
								break;
						}

						break;
					case 0x02:
						switch(dev.subclass_id) {
							case 0x00:
								dev.description = "Ethernet Controller";
								break;
							default:
								dev.description = "Network Controller";
								break;
						}

						break;
					case 0x03:
						dev.description = "Display Controller";
						break;
					case 0x04:
						switch(dev.subclass_id) {
							case 0x00:
								dev.description = "Multimedia Video Controller";
								break;
							case 0x01:
								dev.description = "Multimedia Audio Controller";
								break;
							case 0x03:
								dev.description = "Audio Device";
								break;
							default:
								dev.description = "Multimedia Controller";
								break;
						}

						break;
					case 0x05:
						dev.description = "Memory Controller";
						break;
					case 0x06:
						switch(dev.subclass_id) {
							case 0x00:
								dev.description = "Host Bridge";
								break;
							case 0x01:
								dev.description = "ISA Bridge";
								break;
							case 0x02:
								dev.description = "EISA Bridge";
								break;
							case 0x04:
								dev.description = "PCI-to-PCI Bridge";
								break;
							case 0x09:
								dev.description = "PCI-to-PCI Bridge";
								break;
							default:
								dev.description = "Bridge Device";
								break;
						}

						break;
					case 0x07:
						switch(dev.subclass_id) {
							case 0x00:
								dev.description = "Serial Controller";
								break;
							case 0x01:
								dev.description = "Parallel Controller";
								break;
							default:
								dev.description = "Simple Communication Controller";
								break;
						}

						break;
					case 0x08:
						switch(dev.subclass_id) {
							case 0x00:
								dev.description = "PIC";
								break;
							case 0x01:
								dev.description = "DMA Controller";
								break;
							case 0x02:
								dev.description = "Timer";
								break;
							case 0x03:
								dev.description = "RTC Controller";
								break;
							default:
								dev.description = "Base System Peripheral";
								break;
						}

						break;
					case 0x09:
						dev.description = "Input Device Controller";
						break;
					case 0x0A:
						dev.description = "Docking Station";
						break;
					case 0x0B:
						dev.description = "Processor";
						break;
					case 0x0C:
						switch(dev.subclass_id) {
							case 0x03:
								switch(dev.prog_if) {
									case 0x00:
										dev.description = "UHCI Controller";
										break;
									case 0x10:
										dev.description = "OHCI Controller";
										break;
									case 0x20:
										dev.description = "EHCI Controller";
										break;
									case 0x30:
										dev.description = "XHCI Controller";
										break;
									case 0xFE:
										dev.description = "USB Device";
										break;
									default:
										dev.description = "USB Controller";
										break;
								}
								break;
							default:
								dev.description = "Serial Bus Controller";
								break;
						}

						break;
					case 0x0D:
						dev.description = "Wireless Controller";
						break;
					case 0x0E:
						dev.description = "Intelligent Controller";
						break;
					case 0x0F:
						dev.description = "Satellite Communication Controller";
						break;
					case 0x10:
						dev.description = "Encryption Controller";
						break;
					case 0x11:
						dev.description = "Signal Processing Controller";
						break;
					case 0x12:
						dev.description = "Processing Accelerator";
						break;
					case 0x13:
						dev.description = "Non-Essential Instrumentation";
						break;
					case 0x14:
						dev.description = "Reserved";
						break;
					case 0x40:
						dev.description = "Co-Processor";
						break;
					case 0x41:
						dev.description = "Reserved";
						break;
					case 0xFF:
						dev.description = "Unassigned Class";
						break;
					default:
						dev.description = "Unassigned Class";
						break;
				}
				kdebug(dev.description);
				kdebug("\r\n");

				// RTL8139
				if (dev.vendor_id == 0x10EC && dev.device_id == 0x8139) {
					rtl8139_init(dev);
				}
			}

		}
	}
}
