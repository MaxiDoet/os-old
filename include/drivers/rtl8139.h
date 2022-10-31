#ifndef _rtl8139_h
#define _rtl8139_h

#include "../include/drivers/pci.h"

void rtl8139_init(pci_dev_descriptor dev);
void rtl8139_send(uint16_t *data, uint32_t len);

#endif
