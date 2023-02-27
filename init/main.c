#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../config.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/gdt.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/idt.h"
#include "../include/kernel/scheduler.h"
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
#include "../include/kernel/dd/dev.h"
#include "../include/kernel/audio/dev.h"
#include "../libc/include/string.h"

#include "../include/drivers/serial.h"
#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"
#include "../include/drivers/rtc.h"
#include "../include/drivers/pit.h"
#include "../include/drivers/ata.h"
#include "../include/drivers/rtl8139.h"
#include "../include/drivers/ac97.h"

#include "../apps/desktop/desktop.h"

#include "../include/lib/datetime.h"
#include "../include/lib/wav.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

extern const void kernel_start;
extern const void kernel_end;

const uint8_t gpt_signature[8] = {0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54};
dd_dev_t *root_dev;
fs_t root_fs;

void probe_root_fs(dd_dev_t *dev)
{
	uint16_t *gpt_table_header_buf = (uint16_t *) malloc(512);
	dd_dev_read(dev, 1, 1, gpt_table_header_buf);
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
	dd_dev_read(dev, gpt_table_header->table_lba, 1, gpt_table_buf);
	gpt_table_entry_t *gpt_table = (gpt_table_entry_t *) gpt_table_buf;

	root_fs.type = FS_TYPE_EXT2;
	root_fs.dev = dev;
	root_fs.data = (uint8_t *) malloc(sizeof(ext2_fs_t));
	ext2_probe(dev, gpt_table[0], (ext2_fs_t *) root_fs.data);
}

/* Print a list of all pci devices found */
void pci_dump()
{
	pci_dev_t *list = (pci_dev_t *) malloc(sizeof(pci_dev_t) * 1024);
	uint16_t list_len = pci_get_device_list(list);

	kdebug("PCI Devices\r\n-----------\r\n");

	for (uint16_t i=0; i < list_len; i++) {
		kdebug("Bus %d | Device %d | Function %d | %s\r\n", list[i].bus, list[i].device, list[i].function, list[i].class_description);
	}

	free(list);
}

void kernel_task()
{
	kdebug("Hey\r\n");
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
	ata_detect();
	root_dev = dd_dev_get(0);

	if (root_dev) {
		kdebug("[kernel] Probe for root filesystem\r\n");
		probe_root_fs(root_dev);
	} else {
		kpanic("No root device");
	}

	kdebug("[kernel] PCI detect\r\n");
	pci_detect();
	pci_dump();

	kdebug("[kernel] Net init\r\n");
	net_init();

	kdebug("[kernel] Audio init\r\n");
	audio_init();

	keyboard_init();
	mouse_init();

	/* Init multitasking and kernel task */
	scheduler_init();
	scheduler_task_create((uint32_t) &kernel_task, "kernel");

	//desktop_init(mbi, &root_fs);
}
