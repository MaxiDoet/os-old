#ifndef _ac97_h
#define _ac97_h

#include "../include/kernel/io/pci.h"

#define AC97_DEBUG

void ac97_init(pci_dev_t pci_dev);
void ac97_play(uint8_t *data, uint32_t size);

#endif
