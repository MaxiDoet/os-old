#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../include/drivers/serial.h"

void kdebug(int data_port, char* str)
{
	int i=0;
        while (str[i]!=0) {
                serial_write(data_port, str[i]);
                i++;
        }
}
