#include <stddef.h>
#include <stdint.h>
#include "../include/kernel/kernel.h"
#include "../include/lib/print.h"
#include "../include/drivers/vgacon.h"
#include "../include/drivers/vga-color.h"
#include "../include/lib/string.h"

void kpanic()
{
	vgacon_clear(VGA_COLOR_BLUE);
        vgacon_x = 33;
        vgacon_y = 15;
        vgacon_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);

	prints("Kernel Panic!");

	char* message;
	message = "Unhandled Exception";

	// Center message
	vgacon_x = (vgacon_width / 2) - (strlen(message) / 2) - 1;
	vgacon_y = 17;

	prints(message);

        for(;;);
}

