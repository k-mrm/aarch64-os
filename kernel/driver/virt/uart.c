#include "memmap.h"
#include "driver/virt/uart.h"
#include "trap.h"

#define UART_DR (UARTBASE + 0x00)
#define UART_FR (UARTBASE + 0x18)
#define UART_IBRD (UARTBASE + 0x24)
#define UART_FBRD (UARTBASE + 0x28)
#define UART_LCRH (UARTBASE + 0x2c)
#define UART_CR (UARTBASE + 0x30)

void disable_uart() {
  REG(UART_CR) = 0;
}

void uartintr() {
  ;
}

static inline bool uart_txff() {
  return REG(UART_FR) & (1 << 5);
}

static inline bool uart_rxfe() {
  return REG(UART_FR) & (1 << 4);
}

void uart_putc(char c) {
  while(uart_txff()) {}
  REG(UART_DR) = c;
}

char uart_getc() {
  while(uart_rxfe()) {}
  return REG(UART_DR);
}

void uart_puts(char *s) {
  char c;
  while((c = *s)) {
    while(uart_txff()) {}
    REG(UART_DR) = c;
    s++;
  }
}

void uart_init() {
  disable_uart();

  new_irq(UART_IRQ, uartintr);

  REG(UART_LCRH) = 0x60;
  REG(UART_CR) = 0x301;  /* RXE, TXE, UARTEN */
}
