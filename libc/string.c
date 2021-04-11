#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/lib/string.h"

int strlen(char* str) {
	int i=0;

	while(str[i] != 0) {
		i++;
	}

	return i;
}

uint8_t strcmp(char* str1, char* str2)
{
	uint8_t i;
	uint8_t len = strlen(str1);

	for (i=0; i<=len; i++) {
		if (str1[i] != str2[i]) return 0;
	}

	return 1;
}

char *strcat(char *dest, char *src)
{
	int len;
	int i;

	len = strlen(dest);

	for (i=0; src[i] != '\0'; i++) {
		dest[len+i] = src[i];
	}

	dest[len+i] = '\0';

	return dest;
}

char* strfmt(char* format, va_list *argsp)
{
	char* result = "";

        char c;
        long n;
        int i=0;

        while(format[i] != '\0') {

		c=format[i];

                result[i] = c;

                i++;
        }

	return result;
}

/* Old
char* strfmt(const char* format, va_list *argsp)
{
	char* result = "";

        char c;
        long n;
        int i=0;

        while(format[i] != '\0') {

		c=format[i];

                if(c == '%') {
                        i++;
                        c=format[i];

                        switch(c) {
                                case 'd':
                                        n = va_arg(*argsp, long);

                                        int base=10;
                                        static char buf[32] = {0};

                                        int j = 30;

                                        for(; n && j ; --j, n /= base)

                                               	buf[j] = "0123456789abcdef"[n % base];

                                        strcat(result, &buf[j+1]);

					strcat(result, "10");

                                        break;
                        }
                } else {
                        result[i] = c;
                }

                i++;
        }

	return result;
}
*/
