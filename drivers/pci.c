#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/pci.h"
#include "../include/asm.h"

uint16_t pci_read_word(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
	uint32_t address;
	uint16_t tmp;

	uint32_t lbus = (uint32_t) bus;
	uint32_t ldevice = (uint32_t) device;
	uint32_t lfunc = (uint32_t) func;

	address = (uint32_t)((lbus << 16) | (ldevice << 11) | (lfunc << 8) | (offset & 0xFC) | (uint32_t) 0x80000000);

	outl(0xCF8, address); // Select address

	tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    	return (tmp);
}
