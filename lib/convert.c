#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/lib/convert.h"

char* hex82str(uint8_t key)
{
	char* out = "00";
    	char* hex = "0123456789ABCDEF";
        out[0] = hex[(key >> 4) & 0xF];
        out[1] = hex[key & 0xF];
        return out;
}

/*
char* hex162str(uint16_t key)
{
	char* str = "";
	char* msb = printh8((key >> 8) & 0xFF);
        char* lsb = printh8( key & 0xFF);

	
}
*/
