#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/sb16.h"
#include "../include/kernel/asm.h"

void sb16_dsp_reset()
{
	outb(SB16_DSP_RESET_PORT, 1); // Set reset to 1
	/* Wait 3µ secs*/
	outb(SB16_DSP_RESET_PORT, 0); // Set reset to 0
}

void sb16_init()
{
	// Reset
	sb16_dsp_reset();

	if(inb(SB16_DSP_READ_PORT) != 0xAA) {
		// Failed to init/reset
	}

	// Turn speaker on
	outb(SB16_DSP_WRITE_PORT, 0xD1);

	outb(0x0A, 5); // Disable channel 1
	outb(0x0C, 1); // Flip-Flop
	outb(0x0B, 0x49); // Set transfer mode: Mode + Channel Number
	outb(0x83, 0x01); // Page number
	outb(0x02, 0x04); // Position low
	outb(0x02, 0x0F); // Position high
	outb(0x03, 0xFF); // Count low
	outb(0x03, 0x0F); // Count high
	outb(0x0A, 1); // Enable channel 1

	outb(SB16_DSP_WRITE_PORT, 0x40); // Set time constants
	outb(SB16_DSP_WRITE_PORT, 165); // 10989 Hz
	outb(SB16_DSP_WRITE_PORT, 0xC0); // Set to 8bit sound
	outb(SB16_DSP_WRITE_PORT, 0x00); // Sound data is mono and unsigned
	outb(SB16_DSP_WRITE_PORT, 0xFE); // Count low
	outb(SB16_DSP_WRITE_PORT, 0x0F); // Count high
}

