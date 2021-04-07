#ifndef _irq_h
#define _irq_h

void irq_install_handler(int irq, void (*handler));
void irq_uninstall_handler(int irq);
void irq_handler(uint8_t irq);

#endif
