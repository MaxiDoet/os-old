#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../bin/shell/shell.h"
#include "../include/kernel/panic.h"
#include "../include/drivers/keyboard.h"
#include "../include/lib/print.h"
#include "../include/lib/string.h"
#include "../include/drivers/pci.h"
#include "../include/drivers/cmos.h"
#include "../include/drivers/vga-color.h"
#include "../include/drivers/vgacon.h"
#include "../include/drivers/serial.h"
#include "./strings.h"

char* shell_read_str() {
	char* buffer="";
	uint8_t key;
	uint8_t i=0;
	uint8_t reading=1;

	while(reading) {
		key = keyboard_read();

		switch(key) {
               		case SCANCODE_A:
				buffer[i] = 'a';
				i++;
                        	printc('a');
				break;
                	case SCANCODE_B:
				buffer[i] = 'b';
				i++;
                      	  	printc('b');
				break;
                	case SCANCODE_C:
				buffer[i] = 'c';
                                i++;
                        	printc('c');
				break;
                	case SCANCODE_D:
				buffer[i] = 'd';
                                i++;
                        	printc('d');
				break;
                	case SCANCODE_E:
				buffer[i] = 'e';
                                i++;
                        	printc('e');
				break;
                	case SCANCODE_F:
				buffer[i] = 'f';
                                i++;
                        	printc('f');
				break;
                	case SCANCODE_G:
				buffer[i] = 'g';
                                i++;
                        	printc('g');
				break;
                	case SCANCODE_H:
				buffer[i] = 'h';
                                i++;
                        	printc('h');
				break;
                	case SCANCODE_I:
				buffer[i] = 'i';
                                i++;
                       	 	printc('i');
				break;
               	 	case SCANCODE_J:
				buffer[i] = 'j';
                                i++;
                        	printc('j');
				break;
                	case SCANCODE_K:
				buffer[i] = 'k';
                                i++;
                        	printc('k');
				break;
                	case SCANCODE_L:
				buffer[i] = 'l';
                                i++;
                        	printc('l');
				break;
                	case SCANCODE_M:
				buffer[i] = 'm';
                                i++;
                        	printc('m');
				break;
                	case SCANCODE_N:
				buffer[i] = 'n';
                                i++;
                       		printc('n');
				break;
                	case SCANCODE_O:
				buffer[i] = 'o';
                                i++;
                        	printc('o');
				break;
                	case SCANCODE_P:
				buffer[i] = 'p';
                                i++;
                        	printc('p');
				break;
                	case SCANCODE_Q:
				buffer[i] = 'q';
                                i++;
                        	printc('q');
				break;
                	case SCANCODE_R:
				buffer[i] = 'r';
                                i++;
                        	printc('r');
				break;
			case SCANCODE_S:
				buffer[i] = 's';
                                i++;
                        	printc('s');
				break;
                	case SCANCODE_T:
				buffer[i] = 't';
                                i++;
                        	printc('t');
				break;
                	case SCANCODE_U:
				buffer[i] = 'u';
                                i++;
                        	printc('u');
				break;
                	case SCANCODE_V:
				buffer[i] = 'v';
                                i++;
                        	printc('v');
				break;
                	case SCANCODE_W:
				buffer[i] = 'w';
                                i++;
                        	printc('w');
				break;
                	case SCANCODE_X:
				buffer[i] = 'x';
                                i++;
                        	printc('x');
				break;
                	case SCANCODE_Y:
				buffer[i] = 'y';
                                i++;
                        	printc('y');
				break;
                	case SCANCODE_Z:
				buffer[i] = 'z';
                                i++;
                        	printc('z');
				break;
                	case SCANCODE_1:
				buffer[i] = '1';
                                i++;
                        	printc('1');
				break;
                	case SCANCODE_2:
				buffer[i] = '2';
                                i++;
                        	printc('2');
				break;
                	case SCANCODE_3:
				buffer[i] = '3';
                                i++;
                        	printc('3');
				break;
                	case SCANCODE_4:
				buffer[i] = '4';
                                i++;
                        	printc('4');
				break;
                	case SCANCODE_5:
				buffer[i] = '5';
                                i++;
                        	printc('5');
				break;
               	 	case SCANCODE_6:
				buffer[i] = '6';
                                i++;
                        	printc('6');
				break;
                	case SCANCODE_7:
				buffer[i] = '7';
                                i++;
                        	printc('7');
				break;
                	case SCANCODE_8:
				buffer[i] = '8';
                                i++;
                        	printc('8');
				break;
                	case SCANCODE_9:
				buffer[i] = '9';
                                i++;
                        	printc('9');
				break;
                	case SCANCODE_0:
				buffer[i] = '0';
                                i++;
                        	printc('0');
				break;
                	case SCANCODE_SPACE:
                        	printc(' ');
				break;
                	case SCANCODE_ENTER:
                        	printc('\n');
				reading=0;
				break;
                	case SCANCODE_BACKSPACE:
				erase(1);
                        	buffer[--i] = ' ';
                        	break;
                	default:
                        	printc('\0');
				break;
		}
	}

	buffer[i]=0;
	return buffer;

}

// Commands

void shell_cmd_pcinfo() {
	// Brute-Force pci scan

	prints("\nPCI Devices:\n");

	int bus, slot;
	for (bus=0; bus < 256; bus++) {
		for (slot=0; slot < 32; slot++) {
			uint16_t vendorId = pci_get_vendor_id(bus, slot, 0);
			uint16_t deviceId = pci_get_device_id(bus, slot, 0);
			uint8_t classCode = pci_get_class_code(bus, slot, 0);
			uint8_t subclass = pci_get_subclass(bus, slot, 0);

			if (vendorId == 0xFFFF || vendorId == 0x00FF) {
				continue;
			}

			switch(classCode) {
				case 0x00:
					prints(" Unclassified PCI Device ");
					break;
				case 0x01:
					prints(" Mass Storage Controller ");
					break;
				case 0x02:
					prints(" Network Controller ");
					break;
				case 0x03:
					prints(" Display Controller ");
					break;
				case 0x04:
					prints(" Multimedia Controller ");
					break;
				case 0x05:
					prints(" Memory Controller ");
					break;
				case 0x06:
					prints(" Bridge Device ");
					break;
				case 0x07:
					prints(" Simple Communication Controller ");
					break;
				case 0x08:
					prints(" Base System Peripheral ");
					break;
				case 0x09:
					prints(" Input Device Controller ");
					break;
				case 0x0A:
					prints(" Docking Station ");
					break;
				case 0x0B:
					prints(" Processor ");
					break;
				case 0x0C:
					prints(" Serial Bus Controller ");
					break;
				case 0x0D:
					prints(" Wireless Controller ");
					break;
				case 0x0E:
					prints(" Intelligent Controller ");
					break;
				case 0x0F:
					prints(" Satellite Communication Controller ");
					break;
				case 0x10:
					prints(" Encryption Controller ");
					break;
				case 0x11:
					prints(" Signal Processing Controller ");
					break;
				case 0x12:
					prints(" Processing Accelerator ");
					break;
				case 0x13:
					prints(" Non-Essential Instrumentation ");
					break;
				case 0x14:
					prints(" Reserved ");
					break;
				case 0x40:
					prints(" Co-Processor ");
					break;
				case 0x41:
					prints(" Reserved ");
					break;
				case 0xFF:
					prints(" Unassigned Class ");
					break;
				default:
					prints(" Unassigned Class ");
					break;
			}

			prints(" VendorID: ");
			printh16(vendorId);
			prints(" DeviceID: ");
			printh16(deviceId);
			prints(" Class: ");
			printh8(classCode);
			prints(" Subclass: ");
			printh8(subclass);
			prints(" \n");

			// Identify devices

			switch(vendorId) {

				case 0x1FA4:
					switch(deviceId) {
						// Virtio Filesystem
						case 0x1049:
							prints("Virtio Filesystem\n");
							break;

						default:
							prints("Unknown Device\n");
					}
					break;

				case 0x10EC:
					switch(deviceId) {
						case 0x8029:
							prints("RTL-8029\n");
							break;
						default:
							prints("Unknown Device\n");
					}
					break;

				case 0x8086:
					switch(deviceId) {
						case 0x7010:
							prints("82371SB PIIX3 IDE [Natoma/Triton II]\n");
							break;
						case 0x7000:
							prints("82371SB PIIX3 ISA [Natoma/Triton II]\n");
							break;
						case 0x1237:
							prints("440FX - 82441FX PMC [Natoma] Chipset\n");
							break;
						case 0x100E:
							prints("82540EM Gigabit Ethernet Controller\n");
							break;
						default:
							prints("Unknown Device\n");
					}
					break;

				case 0x1013:
                                        switch(deviceId) {
                                                case 0x00b8:
                                                        prints("Cirrus Logic GD5446\n");
                                                        break;
                                                default:
                                                  	prints("Unknown Device\n");
                                        }
                                        break;

				default:
					prints("Unknown Device\n");
			}
		}
	}
}


void shell_cmd_cmosdump()
{
        // CMOS Dump
        prints("CMOS Dump:\n");

        prints(" RTC: ");

        printi(cmos_rtc_decode(cmos_read(4)));
        prints(":");

        /* If minute is less than 10 print leading zero */
        if (cmos_rtc_decode(cmos_read(2)) < 10) {
                prints("0");
        }

        printi(cmos_rtc_decode(cmos_read(2)));
        prints(":");
        printi(cmos_rtc_decode(cmos_read(0)));

        prints(" ");
        printi(cmos_rtc_decode(cmos_read(7)));
        prints("/");
        printi(cmos_rtc_decode(cmos_read(8)));
        prints("/");
        printi(cmos_rtc_decode(cmos_read(9)));
        prints("\n");
}

void shell_cmd_clear()
{
	vgacon_clear(VGA_COLOR_BLACK);
}

void shell_cmd_serial()
{
	serial_init();
        prints("Serial COM1\n");

	serial_init();

	prints("RX(0) or TX(1)?: ");

	char* choice = shell_read_str();

	if (strcmp("0", choice)) {
		while(1) {
			printc((char)serial_read());
		}
	} else if (strcmp("1", choice)) {
		while(1) {
			char* str;
               		prints("TX-> ");
                	str = shell_read_str();

                	uint8_t len = strlen(str);
                	int i;
                	for (i=0; i<len; i++) {
                        	serial_write(str[i]);
                	}
        	}
	} else {
		prints("Invalid mode!");
		return;
	}
}

void shell_cmd_panic()
{
	kpanic(0);
}

void shell_cmd_test() {
	vgacon_clear(VGA_COLOR_BLACK);

	enum vga_color colors[] = {VGA_COLOR_RED, VGA_COLOR_GREEN, VGA_COLOR_YELLOW, VGA_COLOR_PURPLE, VGA_COLOR_BLUE, VGA_COLOR_WHITE};

	int i=0;
	while(1) {
		if(i==5) i=0;
		vgacon_set_color(colors[i], VGA_COLOR_BLACK);
		prints("#");
		i++;
	}
}

void shell_cmd_divzero() {
	// This should result in a kernel panic
	printi(5/0);
	prints("Looks like the exeption got catched!");
}

void shell_print_prefix()
{
	prints("> ");
}

void shell_handle_cmd(char* cmd)
{
	if(strcmp("help", cmd)) {
		//prints("Help:\n pcinfo Prints all pci devices\n cmosdump dumps cmos registers\n clear blanks the screen\n panic\n serial opens a serial terminal\n test displays some colored strings\n divzero crashes the kernel\n");
		prints(help_text);
	} else if(strcmp("pcinfo", cmd)) {
		shell_cmd_pcinfo();
	} else if(strcmp("cmosdump", cmd)) {
		shell_cmd_cmosdump();
	} else if(strcmp("clear", cmd)) {
		shell_cmd_clear();
	} else if(strcmp("panic", cmd)) {
		shell_cmd_panic();
	} else if(strcmp("serial", cmd)) {
		shell_cmd_serial();
	} else if(strcmp("test", cmd)) {
		shell_cmd_test();
	} else if(strcmp("divzero", cmd)) {
		shell_cmd_divzero();
	} else {
		prints("Invalid command!\n");
	}
}

void shell_main() {
	char* cmd;

	prints("Shell\n");
	shell_print_prefix();

	while(1) {
		cmd = shell_read_str();
		shell_handle_cmd(cmd);
		shell_print_prefix();
	}

}
