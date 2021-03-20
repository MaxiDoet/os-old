#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../bin/shell.h"
#include "../include/keyboard.h"
#include "../include/print.h"
#include "../include/string.h"

void shell_print_prefix()
{
	prints("> ");
}

void shell_handle_cmd(char* cmd)
{
	printi(strlen(cmd));
	prints(" ");
	prints(cmd);

	if(cmd == "aa") {
		prints("This is a test!");
	}
}

void shell_main() {
	shell_print_prefix();

	uint8_t key;

	while(1) {
		key = keyboard_read();

		switch(key) {
               		case SCANCODE_A:
                        	printc('a');
				break;
                	case SCANCODE_B:
                      	  	printc('b');
				break;
                	case SCANCODE_C:
                        	printc('c');
				break;
                	case SCANCODE_D:
                        	printc('d');
				break;
                	case SCANCODE_E:
                        	printc('e');
				break;
                	case SCANCODE_F:
                        	printc('f');
				break;
                	case SCANCODE_G:
                        	printc('g');
				break;
                	case SCANCODE_H:
                        	printc('h');
				break;
                	case SCANCODE_I:
                       	 	printc('i');
				break;
               	 	case SCANCODE_J:
                        	printc('j');
				break;
                	case SCANCODE_K:
                        	printc('k');
				break;
                	case SCANCODE_L:
                        	printc('l');
				break;
                	case SCANCODE_M:
                        	printc('m');
				break;
                	case SCANCODE_N:
                       		printc('n');
				break;
                	case SCANCODE_O:
                        	printc('o');
				break;
                	case SCANCODE_P:
                        	printc('p');
				break;
                	case SCANCODE_Q:
                        	printc('q');
				break;
                	case SCANCODE_R:
                        	printc('r');
				break;
			case SCANCODE_S:
                        	printc('s');
				break;
                	case SCANCODE_T:
                        	printc('t');
				break;
                	case SCANCODE_U:
                        	printc('u');
				break;
                	case SCANCODE_V:
                        	printc('v');
				break;
                	case SCANCODE_W:
                        	printc('w');
				break;
                	case SCANCODE_X:
                        	printc('x');
				break;
                	case SCANCODE_Y:
                        	printc('y');
				break;
                	case SCANCODE_Z:
                        	printc('z');
				break;
                	case SCANCODE_1:
                        	printc('1');
				break;
                	case SCANCODE_2:
                        	printc('2');
				break;
                	case SCANCODE_3:
                        	printc('3');
				break;
                	case SCANCODE_4:
                        	printc('4');
				break;
                	case SCANCODE_5:
                        	printc('5');
				break;
               	 	case SCANCODE_6:
                        	printc('6');
				break;
                	case SCANCODE_7:
                        	printc('7');
				break;
                	case SCANCODE_8:
                        	printc('8');
				break;
                	case SCANCODE_9:
                        	printc('9');
				break;
                	case SCANCODE_0:
                        	printc('0');
				break;
                	case SCANCODE_SPACE:
                        	printc(' ');
				break;
                	case SCANCODE_ENTER:
                        	printc('\n');
				//shell_handle_cmd(buffer);
				//buffer="";
				shell_print_prefix();
				break;
                	case SCANCODE_BACKSPACE:
                        	erase(1);
                        	break;
                	default:
                        	printc('\0');
				break;
		}
	}
}
