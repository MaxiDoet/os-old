#include "./include/string.h"
#include "../include/kernel/kernel.h"

int memcmp(const void* aptr, const void* bptr, size_t size) {
        const unsigned char* a = (const unsigned char*) aptr;
        const unsigned char* b = (const unsigned char*) bptr;
        for (size_t i = 0; i < size; i++) {
                if (a[i] < b[i])
                        return -1;
                else if (b[i] < a[i])
                        return 1;
        }
        return 0;
}

void* memcpy(void* dstptr, const void* srcptr, size_t size) {
        unsigned char* dst = (unsigned char*) dstptr;
        const unsigned char* src = (const unsigned char*) srcptr;
        for (size_t i = 0; i < size; i++)
                dst[i] = src[i];
        return dstptr;
}

void* memmove(void* dstptr, const void* srcptr, size_t size) {
        unsigned char* dst = (unsigned char*) dstptr;
        const unsigned char* src = (const unsigned char*) srcptr;
        if (dst < src) {
                for (size_t i = 0; i < size; i++)
                        dst[i] = src[i];
        } else {
                for (size_t i = size; i != 0; i--)
                        dst[i-1] = src[i-1];
        }
        return dstptr;
}

void* memset(void* bufptr, int value, size_t size) {
        unsigned char* buf = (unsigned char*) bufptr;
        for (size_t i = 0; i < size; i++)
                buf[i] = (unsigned char) value;
        return bufptr;
}

size_t strlen(const char* str) {
        size_t len = 0;
        while (str[len])
                len++;
        return len;
}

size_t strsplit(char *str, char c)
{
	int i=0;

	while(*str != '\0') {
		if (*str == c) {
			i++;
		}

		str++;
	}

	return i;
}

int strcmp(char *str1, char *str2)
{
	int result = 0;

	while (result == 0) {
		if (*str1 > *str2) {
			result = 1;
		} else if (*str1 < *str2) {
			result = -1;
		}

		if (*str1 == '\0') {
			break;
		}

		str1++;
		str2++;
	}

	return result;
}

void strfmt(char *str, char* format, ...)
{
	va_list args;

	va_start(args, format);
	int str_index = 0;

	char c;
    long n;
    int i=0;
	char* s;
	char cc;

	int base;
	char prefix;

    while(format[i] != '\0') {
		c=format[i];

        if(c == '%') {
        	i++;
            c=format[i];

            switch(c) {
				case '%':
					str[str_index] = '%';
					break;
                case 'd':
					prefix=' ';
					base=10;
                    goto print_num;
				case 's':
					s = va_arg(args, char *);
					memcpy((char *) &str[str_index], s, strlen(s));
					break;
				case 'x':
					prefix=' ';
					base=16;
                    goto print_num;

				print_num:
					str[str_index] = prefix;

					n = va_arg(args, long);

					if (n==0) str[str_index++] = '0';

                    static char buf[32] = {0};

                    int j = 30;

                    for(; n && j ; --j, n /= base)
						buf[j] = "0123456789abcdef"[n % base];
						memcpy((char *) &str[str_index], (char *) &buf[j + 1], strlen(&buf[j + 1]));	
						str_index += strlen(&buf[j + 1]) - 1;					
					break;
			}
        } else {
            str[str_index] = c;
		}

        i++;
		str_index++;
	}

	va_end(args);
}
