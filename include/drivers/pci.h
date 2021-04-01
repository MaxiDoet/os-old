#ifndef _pci_h
#define _pci_h

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

uint32_t pci_read_config_dword(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
uint16_t pci_read_config_word(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
uint16_t pci_get_vendor_id(uint8_t bus, uint8_t device, uint8_t func);
uint16_t pci_get_device_id(uint8_t bus, uint8_t device, uint8_t fuc);
uint8_t pci_get_class_code(uint8_t bus, uint8_t device, uint8_t func);
uint8_t pci_get_subclass(uint8_t bus, uint8_t device, uint8_t func);

#endif
