#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/pci.h"
#include "../include/kernel/asm.h"
#include "../include/kernel/kernel.h"

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

				}

				kdebug("[pci] Bus: %d Slot: %d Func: %d Vendor: %x Device: %x ", bus_num, slot_num, function_num, dev.vendor_id, dev.device_id);

				switch(dev.class_id) {
					case 0x00:
						kdebug("Unclassified PCI Device");
						break;
					case 0x01:
						kdebug("Mass Storage Controller");
						break;
					case 0x02:
						kdebug("Network Controller");
						break;
					case 0x03:
						kdebug("Display Controller");
						break;
					case 0x04:
						kdebug("Multimedia Controller");
						break;
					case 0x05:
						kdebug("Memory Controller");
						break;
					case 0x06:
						kdebug("Bridge Device");
						break;
					case 0x07:
						kdebug("Simple Communication Controller");
						break;
					case 0x08:
						kdebug("Base System Peripheral");
						break;
					case 0x09:
						kdebug("Input Device Controller");
						break;
					case 0x0A:
						kdebug("Docking Station");
						break;
					case 0x0B:
						kdebug("Processor");
						break;
					case 0x0C:
						kdebug("Serial Bus Controller");
						break;
					case 0x0D:
						kdebug("Wireless Controller");
						break;
					case 0x0E:
						kdebug("Intelligent Controller");
						break;
					case 0x0F:
						kdebug("Satellite Communication Controller");
						break;
					case 0x10:
						kdebug("Encryption Controller");
						break;
					case 0x11:
						kdebug("Signal Processing Controller");
						break;
					case 0x12:
						kdebug("Processing Accelerator");
						break;
					case 0x13:
						kdebug("Non-Essential Instrumentation");
						break;
					case 0x14:
						kdebug("Reserved");
						break;
					case 0x40:
						kdebug("Co-Processor");
						break;
					case 0x41:
						kdebug("Reserved");
						break;
					case 0xFF:
						kdebug("Unassigned Class");
						break;
					default:
						kdebug("Unassigned Class");
						break;
				}
				kdebug("\r\n");
			}

		}
	}
}
