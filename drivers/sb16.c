#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/sb16.h"
#include "../include/kernel/asm.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/irq.h"

/* Some testing
uint16_t block_size = 44100 / 100 * 2; // 44100 samples per second Note: We need two buffers!
uint16_t buffer_size = block_size * 2 * 2

uint8_t buffer[buffer_size] = {0};

uint16_t bufferOffset;
uint16_t bufferSegment;
*/

void sb16_reset()
{
	outb(SB16_DSP_RESET_PORT, 1); // Set reset to 1
	/* Wait 3µ secs*/
	outb(SB16_DSP_RESET_PORT, 0); // Set reset to 0
}

void sb16_write(uint8_t data)
{
	while((inb(SB16_DSP_WRITE_PORT) & 0x80) != 0);
	outb(SB16_DSP_WRITE_PORT, data);
}

uint8_t sb16_read()
{
	while((inb(SB16_DSP_STATUS_PORT) & 0x80) == 0);
	return inb(SB16_DSP_READ_PORT);
}

void sb16_irq_handler()
{
	kdebug("SB16 IRQ!\r\n");

	kdebug("SB16: Init dma start after irq\r\n");

	outb(0x0A, 5); // Disable channel 1
        outb(0x0C, 1); // Flip-Flop
        outb(0x0B, 0x49); // Set transfer mode: Mode + Channel Number | Modes: Single: 0x48, Auto: 0x58
        //outb(0x83, address); // Page number
        outb(0x02, 0x00); // Position low
        outb(0x02, 0x00); // Position high
        outb(0x03, 0xFF); // Count low
        outb(0x03, 0x1F);
        outb(0x0A, 1); // Enable channel 1

	kdebug("SB16: Init dma end after irq\r\n");
	kdebug("SB16: Init dsp start after irq\r\n");

	sb16_write(0x14);
        sb16_write(0xFF); /* 8Kb */
        sb16_write(0x1F);

	kdebug("SB16: Init dsp end after irq\r\n");
}

void sb16_init()
{
	// Reset
	sb16_reset();

	if(sb16_read() != 0xAA) {
		kdebug("Failed to init SoundBlaster 16!\r\n");
	}

	// Set irq
	outb(SB16_DSP_MIXER_PORT, 0x80);
	outb(SB16_DSP_MIXER_DATA_PORT, 0x02); // IRQ5

	irq_install_handler(5, sb16_irq_handler);


	// Turn speaker on
	sb16_write(0xD1);

	kdebug("SB16: Init dma start\r\n");

	// Init dma
	outb(0x0A, 5); // Disable channel 1
	outb(0x0C, 1); // Flip-Flop
	outb(0x0B, 0x49); // Set transfer mode: Mode + Channel Number | Modes: Single: 0x48, Auto: 0x58
	outb(0x83, 0x01); // Page number
	//outb(0x02, address); // Position low
	//outb(0x02, address >> 8); // Position high
	outb(0x03, 0xFF); // Count low
	outb(0x03, 0x1F);
	outb(0x0A, 1); // Enable channel 1

	kdebug("SB16: Init dma end\r\n");
	kdebug("SB16: Init dsp start\r\n");

	/*
	outb(SB16_DSP_WRITE_PORT, 0x41); // Set samplerate
	outb(SB16_DSP_WRITE_PORT, 0xAC);
	outb(SB16_DSP_WRITE_PORT, 0x44);
	*/

	sb16_write(0x40); // Set time constants
	sb16_write(165);
	sb16_write(0xC0); // Set to 8bit sound
	sb16_write(0x00); // Sound data is mono and unsigned
	//sb16_write(0x14);
	sb16_write(0xFF); /* 8Kb */
	sb16_write(0x1F);

	kdebug("SB16: Init dsp end\r\n");
}

int sb16_probe()
{
	sb16_reset();

	if(sb16_read() != 0xAA) {
                return -1;
        }

	sb16_write(0xE1); // Get dsp version major/minor

	uint8_t major_version;
	uint8_t minor_version;

	major_version=sb16_read();
	minor_version=sb16_read();

	kdebug("Detected Soundblaster 16 V%d.%d\r\n", major_version, minor_version);

	return 0;
}
