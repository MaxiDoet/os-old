#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../config.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/gdt.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/idt.h"
#include "../include/kernel/task.h"
#include "../include/kernel/multiboot.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/mem/pmm.h"
#include "../include/kernel/mem/heap.h"
#include "../include/kernel/console.h"
#include "../include/kernel/io/pci.h"
#include "../include/kernel/fs/ext2.h"
#include "../include/kernel/fs/mbr.h"
#include "../include/kernel/fs/fs.h"
#include "../include/kernel/io/io.h"
#include "../include/kernel/net/net.h"
#include "../include/kernel/platform.h"
#include "../include/kernel/audio/dev.h"
#include "../libc/include/string.h"

#include "../include/drivers/serial.h"
#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"
#include "../include/drivers/rtc.h"
#include "../include/drivers/pit.h"
#include "../include/drivers/ata.h"
#include "../include/drivers/rtl8139.h"

#include "../apps/desktop/desktop.h"

#include "../include/lib/datetime.h"
#include "../include/lib/wav.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

extern const void kernel_start;
extern const void kernel_end;

const uint8_t gpt_signature[8] = {0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54};
ata_dev_t root_dev;
fs_t root_fs;

void probe_root_fs(ata_dev_t *dev)
{
	uint16_t *gpt_table_header_buf = (uint16_t *) malloc(ATA_SECTOR_SIZE);
	ata_pio_read(dev, 1, 1, gpt_table_header_buf);
	gpt_table_header_t *gpt_table_header = (gpt_table_header_t *) gpt_table_header_buf;

	/* Check GPT signature */
	for (int i=0; i < 8; i++) {
		if (gpt_table_header->signature[i] != gpt_signature[i]) {
			kdebug("[rootfs] invalid gpt signature\r\n");
			return;
		}
	}

	uint8_t sector_count = (gpt_table_header->table_entry_count * gpt_table_header->table_entry_size) / ATA_SECTOR_SIZE + 1;

	uint16_t *gpt_table_buf = (uint16_t *) malloc(sector_count * ATA_SECTOR_SIZE);
	ata_pio_read(dev, gpt_table_header->table_lba, sector_count, gpt_table_buf);
	gpt_table_entry_t *gpt_table = (gpt_table_entry_t *) gpt_table_buf;

	root_fs.type = FS_TYPE_EXT2;
	root_fs.dev = dev;
	root_fs.data = (uint8_t *) malloc(sizeof(ext2_fs_t));
	ext2_probe(dev, gpt_table[0], (ext2_fs_t *) root_fs.data);
}

void kmain(unsigned long magic, unsigned long mbi_addr)
{
	multiboot_info_t *mbi;
	mbi = (multiboot_info_t *) mbi_addr;

	kdebug("[kernel] GDT init\r\n");
    gdt_setup();
    kdebug("[kernel] IDT init\r\n");
    idt_install();

	// Init serial debug
	serial_init(DEBUG_PORT);

	// Init kernel heap
	uint32_t heap_start = mbi->mem_upper + 10*1024*1024;
	uint32_t heap_size = 100000000;
	kdebug("[kernel] Heap init | Start: %x | Size: %x\r\n", heap_start, heap_size);
	heap_init(heap_start, heap_size);

	// Init PIT
	pit_init();

	kdebug("[kernel] Platform init\r\n");
	platform_init();

	kdebug("[kernel] ATA init\r\n");

	if (ata_init(&root_dev, true, true)) {
		kdebug("[kernel] root device: ATA->Primary Master\r\n");
	} else if (ata_init(&root_dev, true, false)) {
		kdebug("[kernel] root device: ATA->Primary Slave\r\n");
	} else if (ata_init(&root_dev, false, true)) {
		kdebug("[kernel] root device: ATA->Secondary Master\r\n");
	} else if (ata_init(&root_dev, false, false)) {
		kdebug("[kernel] root device: ATA->Secondary Slave\r\n");
	}

	if (root_dev.ready) {
		kdebug("[kernel] Probe for root filesystem\r\n");
		probe_root_fs(&root_dev);
	}

	pci_scan();

	kdebug("[kernel] Net init\r\n");
	net_init();
	
	kdebug("[kernel] Audio init\r\n");
	audio_init();

	keyboard_init();
	mouse_init();

	/*
	uint8_t *test_buf = (uint8_t *) malloc(25548514);
	memset(test_buf, 0x00, 25548514);

	if (fs_read(&root_fs, "/audio.wav", test_buf)) {
		audio_dev_play(test_buf, 25548514);
	}
	free(test_buf);
	*/

	//tasking_init();

	desktop_init(mbi, &root_fs);
}
