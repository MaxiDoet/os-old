#ifndef _irq_h
#define _irq_h

uint8_t irq_install_handler(int irq, void (*handler));
bool irq_is_used(int irq);
void irq_uninstall_handler(int irq);
void irq_handler(uint8_t irq);
void irq_print_map();

#endif
