#ifndef _ata_h
#define _ata_h

#include <stdint.h>

typedef struct {
	bool primary;
	bool master;
	uint16_t io_base;

	bool ready;
} ata_dev_t;

#define ATA_SECTOR_SIZE 512

uint8_t ata_init(ata_dev_t *dev, bool primary, bool master);
void ata_pio_read(ata_dev_t *dev, uint32_t lba, uint8_t sector_count, uint16_t *buf);
//void ata_pio_write(ata_dev_t dev, uint32_t sector, uint16_t* buf, uint32_t count);
//void ata_pio_flush(ata_dev_t dev);

#endif
