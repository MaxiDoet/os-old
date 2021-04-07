#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/pci.h"
#include "../include/kernel/asm.h"

uint32_t nambar;
uint32_t nabmbar;

struct buf_desc {
	void *buffer;
	unsigned short length;
 	int reserved : 14;
 	unsigned int bup : 1;
 	unsigned int ioc : 1;
} __attribute__((packed));

struct buf_desc *BufDescList;

void ac97_init(uint32_t bar0, uint32_t bar1)
{
	nambar = bar0;
	nabmbar = bar1;

	// Reset
	outl(nambar + 0x0000, (uint16_t) 42);

	outl(nabmbar + 0x0060, (uint16_t) 0x02);

	// Set volume
	outl(nambar + 0x0002, (uint16_t) (0<<8) | 0); // Global
	outl(nambar + 0x0018, (uint16_t) (0<<8) | 0); // PCM

	// Set samplerate
	outl(nambar + 0x002A, (uint16_t) inl(nambar + 0x002A) | 1);
	outl(nambar + 0x002C, (uint16_t) 44100); // Global
	outl(nambar + 0x0032, (uint16_t) 44100); // Stereo

	int size;
	int i;
	int final;

	for (i = 0; (i < 32) && size; i++) {
		BufDescList[i].buffer = nambar;

		if (size >= 0x20000)
 		{
   			BufDescList[i].length = 0xFFFE;
   			size -= 0x20000;
 		} else {
			BufDescList[i].length = size >> 1;
			size = 0;
		}

 		BufDescList[i].ioc = 1;
 		if (size) {
			BufDescList[i].bup = 0;
		} else {
			BufDescList[i].bup = 1;
   			final = i;
		}
	}

	outl(nabmbar + 0x0010, (uint32_t) &BufDescList);
	outb(nabmbar + 0x0015, final);
	outb(nabmbar + 0x001B, 0x15);
}
