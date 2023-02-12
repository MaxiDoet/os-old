#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/dd/dev.h"
#include "../include/drivers/ata.h"

dd_dev_t dd_dev_list[1024];
uint16_t dd_dev_list_index = 0;

void dd_dev_add(dd_dev_t dev)
{
    dd_dev_list[dd_dev_list_index++] = dev;
}

dd_dev_t *dd_dev_get(uint16_t id)
{
    return &dd_dev_list[id];
}

dd_return_t dd_dev_read(dd_dev_t *dev, uint32_t addr, uint32_t size, void *buffer)
{
    switch(dev->bus_type) {
        case DD_BUS_TYPE_ATA:
            return ata_pio_read((ata_dev_t *) dev->driver_data, addr, size, (uint16_t *) buffer);
    }
}