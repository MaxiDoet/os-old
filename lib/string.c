#include "../include/string.h"

int strlen(char* str) {
	int i=0;

	while(str[i] != 0) {
		i++;
	}

	return i;
}
