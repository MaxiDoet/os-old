#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/pci.h"
#include "../include/kernel/asm.h"

uint32_t pci_read_dword(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
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

uint16_t pci_read_word(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
	uint32_t tmp;
	tmp = pci_read_dword(bus, device, func, offset);

	return (tmp >> ((offset & 3) * 8)) & 0xffff;
}

pci_dev_descriptor pci_get_dev_descriptor(uint8_t bus, uint8_t device, uint8_t func)
{
	pci_dev_descriptor dev;

	dev.bus = bus;
	dev.device = bus;
	dev.function = func;

	dev.vendor_id = pci_read_dword(bus, device, func, 0x00);
	dev.device_id = pci_read_dword(bus, device, func, 0x00) >> 16;

	dev.class_id = pci_read_word(bus, device, func, 0x08) >> 24 & 0xFF;
	dev.subclass_id = pci_read_word(bus, device, func, 0x08) >> 16 & 0xFF;

	return dev;
}

pci_bar_descriptor pci_get_bar_descriptor(uint8_t bus, uint8_t device, uint8_t func, int barNum) {
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

void pci_probe()
{
	int busNum;
	int deviceNum;
	int functionNum;
	int barNum;

	for (busNum=0; busNum < 10; busNum++) {
		for (deviceNum=0; deviceNum < 32; deviceNum++) {
			for (functionNum=0; functionNum < 6; functionNum++) {
				pci_dev_descriptor dev = pci_get_dev_descriptor(busNum, deviceNum, functionNum);

				if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF) continue;

				for(barNum = 0; barNum < 6; barNum++) {
					pci_bar_descriptor bar = pci_get_bar_descriptor(busNum, deviceNum, functionNum, barNum);

					if (bar.address && (bar.type == PCI_BAR_LAYOUT_IO)) {
						dev.io_base = (uint32_t)bar.address;
					}
				}
			}

		}
	}
}
