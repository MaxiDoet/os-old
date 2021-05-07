#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "../include/lib/string.h"
#include "../include/kernel/kernel.h"
#include "../include/drivers/serial.h"
#include "../config.h"

void puts(int data_port, char* str)
{
	int i=0;
	while(str[i] != '\0') {
		serial_write(data_port, str[i]);
		i++;
	}
}

/* Add more functions and add padding to hex numbers */

void kdebug(char* format, ...)
{
	va_list args;

	va_start(args, format);

	char c;
        long n;
        int i=0;
	char* s;
	char cc;

	int base;
	char* prefix;

        while(format[i] != '\0') {

                c=format[i];

                if(c == '%') {
                        i++;
                        c=format[i];

                        switch(c) {
				case '%':
					puts(DEBUG_PORT, "%");
					break;
                                case 'd':
					prefix="";
					base=10;
                                        goto print_num;
				case 's':
					s = va_arg(args, char *);
					puts(DEBUG_PORT, s);
					break;
				/*
				case 'c':
					cc = va_arg(args, int);
					puts(DEBUG_PORT, cc + '\0');
					break;
				*/
				case 'x':
					prefix="";
					base=16;
                                        goto print_num;

				print_num:
					puts(DEBUG_PORT, prefix);

					n = va_arg(args, long);

					if (n==0) kdebug("0");

                                        static char buf[32] = {0};

                                        int j = 30;

                                        for(; n && j ; --j, n /= base)

                                                buf[j] = "0123456789abcdef"[n % base];

					puts(DEBUG_PORT, &buf[j+1]);
                        }
                } else {
                        serial_write(DEBUG_PORT, c);
                }

                i++;
        }

	va_end(args);

}
