#include "./include/string.h"

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
