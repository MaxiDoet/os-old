#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "../include/kernel/kernel.h"
#include "../include/drivers/serial.h"

void puts(int data_port, char* str)
{
	int i=0;
	while(str[i] != '\0') {
		serial_write(data_port, str[i]);
		i++;
	}
}

void kdebug(int data_port, const char* format, ...)
{
	va_list *args;

	char c;
	long n;
	int i=0;

	while((c = format[i]) != '\0') {
		if(c == '%') {
			i++;
			c=format[i];

			switch(c) {
				case 'd':
					n = va_arg(*args, long);

					int base=10;
        				static char buf[32] = {0};

        				int j = 30;

        				for(; n && j ; --j, n /= base)

                				buf[j] = "0123456789abcdef"[n % base];

        				kdebug(data_port, &buf[j+1]);

					break;
			}
		} else {
			serial_write(data_port, c);
		}

		i++;
	}
}
