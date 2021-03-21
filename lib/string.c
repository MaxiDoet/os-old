#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/string.h"

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
