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

#define ATA_SECTOR_SIZE 512

#define ATA_PRIMARY_MASTER 0x1F0
#define ATA_PRIMARY_SLAVE 0x170
#define ATA_SECONDARY_MASTER 0x1E8
#define ATA_SECONDARY_SLAVE 0x168

uint8_t ata_init(ata_dev_t *dev, uint16_t port_base, bool master);
void ata_find(ata_dev_t *dev);
uint8_t ata_pio_sleep(ata_dev_t dev);
void ata_pio_read(ata_dev_t dev, uint32_t lba, int sector_count, uint16_t *buf);
void ata_pio_write(ata_dev_t dev, uint32_t sector, uint16_t* buf, uint32_t count);
void ata_pio_flush(ata_dev_t dev);

void read_sectors_ATA_PIO(uint32_t target_address, uint32_t LBA, uint8_t sector_count);

#endif
