#ifndef MONO_UART_H
#define MONO_UART_H

#include "mono.h"
#include "memmap.h"

#define UARTBASE(n) (PERIPHERAL_BASE + 0x201000 + (n) * 0x200)
#define UART_DR(n)  (UARTBASE(n) + 0x00)
#define UART_FR(n)  (UARTBASE(n) + 0x18)
#define UART_IBRD(n)  (UARTBASE(n) + 0x24)
#define UART_FBRD(n)  (UARTBASE(n) + 0x28)
#define UART_LCRH(n)  (UARTBASE(n) + 0x2c)
#define UART_CR(n)  (UARTBASE(n) + 0x30)

#define UART_FREQ 48000000
#define UART_BAUD 115200

void uart_init(int n);
void uart_putc(int n, char c);
void uart_puts(int n, char *s);

#endif
