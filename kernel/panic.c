#include <stddef.h>
#include <stdint.h>
#include "../include/drivers/serial.h"
#include "../include/kernel/kernel.h"
#include "../config.h"

void kpanic(char* msg)
{
	/* Init serial in case panic happes before serial init */
    serial_init(DEBUG_PORT);

	kdebug("[kernel] Panic | %s\r\n", msg);

	for(;;);
}