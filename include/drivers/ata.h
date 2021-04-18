#ifndef _ata_h
#define _ata_h

typedef struct {
	uint16_t data_port;
	uint16_t error_port;
	uint16_t sector_count_port;
	uint16_t lba_low_port;
	uint16_t lba_mid_port;
	uint16_t lba_high_port;
	uint16_t device_select_port;
	uint16_t command_port;
	uint16_t control_port;
	bool master;
} ata_dev_t;

ata_dev_t ata_init(uint16_t port_base, bool master);
uint8_t ata_busy_wait(ata_dev_t dev);
uint16_t ata_read(ata_dev_t dev, uint32_t sector, int count);
void ata_write(ata_dev_t dev, uint32_t sector, uint8_t* data, uint32_t count);
void ata_flush(ata_dev_t dev);

#endif
