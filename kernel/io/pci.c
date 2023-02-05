#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/io/pci.h"
#include "../include/kernel/io/io.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/mem/heap.h"
#include "../include/drivers/rtl8139.h"
#include "../include/drivers/ac97.h"
#include "../include/drivers/hda.h"
#include "../libc/include/string.h"

pci_dev_t pci_device_list[1024];
uint16_t pci_device_list_index = 0;

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

pci_dev_t pci_get_dev_descriptor(uint16_t bus, uint16_t device, uint16_t func)
{
	pci_dev_t dev;

	dev.bus = bus;
	dev.device = device;
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
				bar.mem_base = (bar_value & ~0xF);
				break;
		}
	} else {
		bar.io_base = (uint32_t) (bar_value & 0xFFFFFFFC);
	}

	return bar;
}

void pci_scan()
{
	int bus_num;
	int slot_num;
	int function_num;
	int bar_num;

	for (bus_num=0; bus_num < 10; bus_num++) {
		for (slot_num=0; slot_num < 32; slot_num++) {
			for (function_num=0; function_num < 6; function_num++) {
				pci_dev_t dev = pci_get_dev_descriptor(bus_num, slot_num, function_num);

				if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF) continue;

				for(bar_num = 0; bar_num < 6; bar_num++) {
					pci_bar_descriptor bar = pci_get_bar_descriptor(bus_num, slot_num, function_num, bar_num);

					dev.bars[bar_num] = bar;
				}

				switch(dev.class_id) {
					case 0x00:
						dev.class_description = "Unclassified PCI Device";
						break;
					case 0x01:
						switch(dev.subclass_id) {
							case 0x00:
								dev.class_description = "SCSI Bus Controller";
								break;
							case 0x01:
								dev.class_description = "IDE Controller";
								break;
							case 0x02:
								dev.class_description = "Floppy Disk Controller";
								break;
							case 0x04:
								dev.class_description = "RAID Controller";
								break;
							case 0x05:
								dev.class_description = "ATA Controller";
								break;
							case 0x06:
								dev.class_description = "Serial ATA Controller";
								break;
							case 0x07:
								dev.class_description = "Serial Attached SCSI Controller";
								break;
							case 0x08:
								dev.class_description = "NVM Controller";
								break;
							default:
								dev.class_description = "Mass Storage Controller";
								break;
						}

						break;
					case 0x02:
						switch(dev.subclass_id) {
							case 0x00:
								dev.class_description = "Ethernet Controller";
								break;
							default:
								dev.class_description = "Network Controller";
								break;
						}

						break;
					case 0x03:
						dev.class_description = "Display Controller";
						break;
					case 0x04:
						switch(dev.subclass_id) {
							case 0x00:
								dev.class_description = "Multimedia Video Controller";
								break;
							case 0x01:
								dev.class_description = "Multimedia Audio Controller";
								break;
							case 0x03:
								dev.class_description = "Audio Device";
								break;
							default:
								dev.class_description = "Multimedia Controller";
								break;
						}

						break;
					case 0x05:
						dev.class_description = "Memory Controller";
						break;
					case 0x06:
						switch(dev.subclass_id) {
							case 0x00:
								dev.class_description = "Host Bridge";
								break;
							case 0x01:
								dev.class_description = "ISA Bridge";
								break;
							case 0x02:
								dev.class_description = "EISA Bridge";
								break;
							case 0x04:
								dev.class_description = "PCI-to-PCI Bridge";
								break;
							case 0x09:
								dev.class_description = "PCI-to-PCI Bridge";
								break;
							default:
								dev.class_description = "Bridge Device";
								break;
						}

						break;
					case 0x07:
						switch(dev.subclass_id) {
							case 0x00:
								dev.class_description = "Serial Controller";
								break;
							case 0x01:
								dev.class_description = "Parallel Controller";
								break;
							default:
								dev.class_description = "Simple Communication Controller";
								break;
						}

						break;
					case 0x08:
						switch(dev.subclass_id) {
							case 0x00:
								dev.class_description = "PIC";
								break;
							case 0x01:
								dev.class_description = "DMA Controller";
								break;
							case 0x02:
								dev.class_description = "Timer";
								break;
							case 0x03:
								dev.class_description = "RTC Controller";
								break;
							default:
								dev.class_description = "Base System Peripheral";
								break;
						}

						break;
					case 0x09:
						dev.class_description = "Input Device Controller";
						break;
					case 0x0A:
						dev.class_description = "Docking Station";
						break;
					case 0x0B:
						dev.class_description = "Processor";
						break;
					case 0x0C:
						switch(dev.subclass_id) {
							case 0x03:
								switch(dev.prog_if) {
									case 0x00:
										dev.class_description = "UHCI Controller";
										break;
									case 0x10:
										dev.class_description = "OHCI Controller";
										break;
									case 0x20:
										dev.class_description = "EHCI Controller";
										break;
									case 0x30:
										dev.class_description = "XHCI Controller";
										break;
									case 0xFE:
										dev.class_description = "USB Device";
										break;
									default:
										dev.class_description = "USB Controller";
										break;
								}
								break;
							default:
								dev.class_description = "Serial Bus Controller";
								break;
						}

						break;
					case 0x0D:
						dev.class_description = "Wireless Controller";
						break;
					case 0x0E:
						dev.class_description = "Intelligent Controller";
						break;
					case 0x0F:
						dev.class_description = "Satellite Communication Controller";
						break;
					case 0x10:
						dev.class_description = "Encryption Controller";
						break;
					case 0x11:
						dev.class_description = "Signal Processing Controller";
						break;
					case 0x12:
						dev.class_description = "Processing Accelerator";
						break;
					case 0x13:
						dev.class_description = "Non-Essential Instrumentation";
						break;
					case 0x14:
						dev.class_description = "Reserved";
						break;
					case 0x40:
						dev.class_description = "Co-Processor";
						break;
					case 0x41:
						dev.class_description = "Reserved";
						break;
					case 0xFF:
						dev.class_description = "Unassigned Class";
						break;
					default:
						dev.class_description = "Unassigned Class";
						break;
				}

				pci_device_list[pci_device_list_index++] = dev;
			}

		}
	}
}

uint16_t pci_get_device_list(pci_dev_t *list)
{
	/* Protect list from being changed */
	memcpy(list, pci_device_list, pci_device_list_index * sizeof(pci_dev_t));

	return pci_device_list_index;
}

/* This function returns a list of pci devices with matching vendor oder device ids 
   If value is 0 then the id is ignored
*/
uint16_t pci_get_device_by_id(pci_dev_t *list, uint16_t vendor_id, uint16_t device_id)
{
	uint16_t list_length = 0;

	for (uint16_t i=0; i < pci_device_list_index; i++) {
		pci_dev_t *dev = &pci_device_list[i];

		if ((vendor_id != 0) && (vendor_id != dev->vendor_id)) {
			continue;
		}

		if ((device_id != 0) && (device_id != dev->device_id)) {
			continue;
		}

		list[list_length++] = *dev;
	}

	return list_length;
}