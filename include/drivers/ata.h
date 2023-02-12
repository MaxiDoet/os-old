#ifndef _ata_h
#define _ata_h

#include <stdint.h>

typedef struct {
	bool primary;
	bool master;
	uint16_t io_base;

	bool ready;

	char serial[20];
	char model[40];
	char firmware_revision[8];
} ata_dev_t;

#define ATA_SECTOR_SIZE 512

#define ATA_RETURN_ERROR		0
#define ATA_RETURN_SUCCESS 		1
#define ATA_RETURN_NOT_READY	2
#define ATA_RETURN_NONE			3

uint8_t ata_pio_read(ata_dev_t *dev, uint32_t lba, uint8_t sector_count, uint16_t *buf);
uint8_t ata_pio_write(ata_dev_t *dev, uint32_t lba, uint16_t* buf, uint8_t sector_count);

void ata_detect();

#endif
