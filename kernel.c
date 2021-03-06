#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "tty.h"
#include "random.h"

void printf(char* str) {
	tty_printf(str);
}

void kernel_main(void) {
	tty_init();

	tty_setcolor(VGA_COLOR_RED, VGA_COLOR_DARKGRAY);
	tty_clear();

	printf("Welcome to my OS!\n");

	int r[2800+1]; 
    int i,k,b,d,c=0; 
 
    for(i=0;i<2800;i++) r[i]=2000; 
 
 
    for(k=2800;k>0;k-=14){ 
        d=0; 
        i=k; 
        for(;;){ 
            d+=r[i]*10000; 
            b=2*i-1; 
            r[i]=d%b; 
            d/=b; 
            i--; 
            if(i==0) break; 
            d*=i; 
        } 
        printf(("%c.4d",(char*)c+d/10000)); 
        c=d%10000; 
    } 
}
