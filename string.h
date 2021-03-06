#ifndef _string_h
#define _string_h
#endif

size_t strlen(char * str)
{
	size_t i = 0;
	while(str[i] != "\0") {
		i++;
	}

	return i + 1;
}
