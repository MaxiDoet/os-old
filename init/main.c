#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../config.h"
#include "../include/kernel/kernel.h"
#include "../include/drivers/serial.h"
#include "../include/drivers/vgacon.h"
#include "../include/drivers/vga-color.h"
#include "../libc/include/mm.h"
#include "../include/kernel/gdt.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/idt.h"
#include "../include/kernel/multiboot.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/pmm.h"
#include "../include/drivers/pci.h"
#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"
#include "../include/drivers/cmos.h"
#include "../include/drivers/sb16.h"
#include "../include/drivers/pit.h"
#include "../include/drivers/ata.h"
#include "../include/kernel/fat16.h"
#include "../include/kernel/asm.h"

#include "../bin/desktop/desktop.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

extern const void kernel_start;
extern const void kernel_end;

void serial_irq_handler()
{
	char c = serial_read(DEBUG_PORT);

	switch(c) {
		case 'i':
			irq_print_map();
			break;
		case 'p':
			pmm_info();
			break;
	}
}

void kmain(unsigned long magic, unsigned long mbi_addr)
{
	multiboot_info_t *mbi;
	mbi = (multiboot_info_t *) mbi_addr;

	int debug_port;

	// Init serial debug
	serial_init(DEBUG_PORT);
	kdebug(BUILD_INFO);
	kdebug("\r\n");

	// For debug commands
	irq_install_handler(4, serial_irq_handler);


	kdebug("[kernel\e[0;37m] GDT init\r\n");
	gdt_setup();
	kdebug("[kernel\e[0;37m] IDT init\r\n");
	idt_install();

	kdebug("[kernel] PMM init\r\n");
	//pmm_init(mbi);
	//pmm_alloc();

	kdebug("[kernel] Heap init\r\n   Start: %x\r\n", mbi->mem_upper);
	mm_init(mbi->mem_upper, 10*1024);

	keyboard_init();
	mouse_init();

	kdebug("[ata] ATA init\r\n");
	ata_dev_t ata = ata_init(0x1F0, true);

	//uint16_t *ata_buf = (uint16_t *) malloc(mm, ATA_SECTOR_SIZE);

	uint32_t* target;

    	read_sectors_ATA_PIO(target, 0x0, 1);

	int i;
    	i = 0;
    	while(i < 128)
    	{
        	kdebug("%x ", target[i] & 0xFF);
        	kdebug("%x ", (target[i] >> 8) & 0xFF);
        	i++;
    	}


	//ata_pio_read(ata, 0x0, 1, ata_buf);

	//fat16_bpb *bpb = (fat16_bpb *) &ata_buf;
	/*
	for (int i=0; i < ATA_SECTOR_SIZE; i++) {
		kdebug("%x ", ata_buf[i]);
	}
	*/

	void *fb = (void *) (unsigned long) mbi->framebuffer_addr;

	pci_probe();
	if(sb16_probe() == 0) {
		sb16_init();
	}

	#ifdef DESKTOP
		desktop_init(mbi->framebuffer_addr, mbi->framebuffer_width, mbi->framebuffer_height, mbi->framebuffer_pitch);
	#endif
}
