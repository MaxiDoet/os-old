#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/asm.h"
#include "../include/drivers/bga.h"

void bga_set_video_mode(int width, int height, int depth)
{
	outb(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED); // Disable VBE extensions

	outb(VBE_DISPI_INDEX_XRES, width); // Width
	outb(VBE_DISPI_INDEX_YRES, height); // Height
	outb(VBE_DISPI_INDEX_BPP, depth); // Depth

	outb(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED); // Enable VBE extensions
}
