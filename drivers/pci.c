#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/pci.h"
#include "../include/kernel/asm.h"

uint32_t pci_read_config_dword(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
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

uint16_t pci_read_config_word(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
	uint32_t tmp;
	tmp = pci_read_config_dword(bus, device, func, offset);

	return (tmp >> ((offset & 3) * 8)) & 0xffff;
}

uint16_t pci_get_vendor_id(uint8_t bus, uint8_t device, uint8_t func)
{
	uint32_t tmp = pci_read_config_dword(bus, device, func, 0);

	return tmp;
}

uint16_t pci_get_device_id(uint8_t bus, uint8_t device, uint8_t func)
{
	uint32_t tmp = pci_read_config_dword(bus, device, func, 0);

        return tmp >> 16;
}

uint8_t pci_get_class_code(uint8_t bus, uint8_t device, uint8_t func)
{
	uint32_t tmp = pci_read_config_dword(bus, device, func, 8);

        return tmp >> 24 & 0xFF;
}

uint8_t pci_get_subclass(uint8_t bus, uint8_t device, uint8_t func)
{
	uint32_t tmp = pci_read_config_dword(bus, device, func, 8);

        return tmp >> 16 & 0xFF;
}
