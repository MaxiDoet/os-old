#ifndef _print_h
#define _print_h

#include "string.h"
#include "tty.h"

char* _int_to_string(int i) {
        char* str;

        while(i != 0) {
                i/=10;
                strcat(str, (char*)'0'+i);
        }

        return str;
}

void prints(char* str)
{
        int i=0;
        while (str[i] != 0) {

                if(str[i] == '\n') {
                        // Newline
                        tty_row++;
                        tty_column=0;

                        if (str[++i] != 0) {
                                continue;
                        } else {
                                break;
                        }
                }

                tty_put(tty_fg_color, tty_bg_color, str[i]);

                i++;
        }
}

void printi(int i)
{
	//prints(("%c", _int_to_string(i)));
	prints('5');
}

#endif
