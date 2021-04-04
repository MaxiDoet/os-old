#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "../include/lib/string.h"
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

void kdebug(int data_port, char* format, ...)
{

	va_list args;

	va_start(args, format);

	char c;
        long n;
        int i=0;
	char* s;
	char cc;

	int base;

        while(format[i] != '\0') {

                c=format[i];

                if(c == '%') {
                        i++;
                        c=format[i];

                        switch(c) {
                                case 'd':
					base=10;
                                        goto print_num;
				case 's':
					s = va_arg(args, char *);
					puts(data_port, s);
					break;
				case 'c':
					cc = va_arg(args, int);
					puts(data_port, (char *)cc);
					break;
				case 'x':
					base=16;
                                        goto print_num;

				print_num:
					n = va_arg(args, long);

                                        static char buf[32] = {0};

                                        int j = 30;

                                        for(; n && j ; --j, n /= base)

                                                buf[j] = "0123456789abcdef"[n % base];

					puts(data_port, &buf[j+1]);
                        }
                } else {
                        serial_write(data_port, c);
                }

                i++;
        }

	va_end(args);

}