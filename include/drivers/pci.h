#ifndef _pci_h
#define _pci_h

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

enum pci_bar_layout_type {
	PCI_BAR_LAYOUT_MEMORYMAPPING,
	PCI_BAR_LAYOUT_IO
};

typedef struct {
	uint32_t io_base;
	uint32_t size;
	enum pci_bar_layout_type type;
} pci_bar_descriptor;

typedef struct {
	uint16_t bus;
	uint16_t device;
	uint16_t function;

	uint16_t vendor_id;
	uint16_t device_id;

	char* description;

	uint8_t class_id;
	uint8_t subclass_id;
	uint8_t prog_if;
	uint8_t revision_id;

	pci_bar_descriptor bars[6];

	uint16_t command;
	uint8_t irq;

} pci_dev_descriptor;

uint32_t pci_read_dword(uint16_t bus, uint16_t device, uint16_t func, uint32_t offset);
uint16_t pci_read_word(uint16_t bus, uint16_t device, uint16_t func, uint16_t offset);
void pci_write_dword(uint16_t bus, uint16_t device, uint16_t func, uint32_t offset, uint32_t value);
void pci_write_word(uint16_t bus, uint16_t device, uint16_t func, uint32_t offset, uint16_t value);
pci_dev_descriptor pci_get_dev_descriptor(uint16_t bus, uint16_t device, uint16_t func);
pci_bar_descriptor pci_get_bar_descriptor(uint16_t bus, uint16_t device, uint16_t func, int barNum);
void pci_scan();
void pci_enable_bus_mastering(pci_dev_descriptor dev);

#endif
