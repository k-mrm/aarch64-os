#ifndef AARCH64_OS_DRIVER_VIRT_UART_H
#define AARCH64_OS_DRIVER_VIRT_UART_H

#ifndef __ASSEMBLER__

#include "kernel.h"

void uart_init(void);
void uart_putc(char c);
void uart_puts(char *s);

#endif

#endif
