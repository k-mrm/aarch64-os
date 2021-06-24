#ifndef MONO_DRIVER_VIRT_UART_H
#define MONO_DRIVER_VIRT_UART_H

#include "mono.h"
#include "memmap.h"

#define UARTBASE 0x09000000

void uart_init(void);
void uart_putc(char c);
void uart_puts(char *s);

#endif
