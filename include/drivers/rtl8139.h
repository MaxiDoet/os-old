#ifndef _rtl8139_h
#define _rtl8139_h

#include "../include/kernel/io/pci.h"

void rtl8139_init(pci_dev_t dev);
void rtl8139_send(uint8_t *data, uint32_t size);

#endif
