#ifndef _shell_h
#define _shell_h

#include "../include/scancodes.h"

//char* help_text = "Help:\n pciscan\n";

void shell_print_prefix();
void shell_handle_cmd(char* cmd);
void shell_main();

#endif
