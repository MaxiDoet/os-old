#ifndef _vfs_h
#define _vfs_h

#include "../include/drivers/ata.h"

void vfs_probe(ata_dev_t dev);
uint8_t vfs_read(char* path, uint16_t *buf);

#endif
