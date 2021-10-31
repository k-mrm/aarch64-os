#include "memmap.h"
#include "driver/virt/uart.h"
#include "trap.h"
#include "log.h"

/* pl011 */

#define UART_DR (UARTBASE + 0x00)
#define UART_FR (UARTBASE + 0x18)
#define FR_RXFE (1 << 4)  /* recieve fifo empty */
#define FR_TXFF (1 << 5)  /* transmit fifo full */
#define FR_RXFF (1 << 6)  /* recieve fifo full */
#define FR_TXFE (1 << 7)  /* transmit fifo empty */
#define UART_IBRD (UARTBASE + 0x24)
#define UART_FBRD (UARTBASE + 0x28)
#define UART_LCRH (UARTBASE + 0x2c)
#define LCRH_FEN  (1 << 4)
#define LCRH_WLEN_8BIT  (3 << 5)
#define UART_CR (UARTBASE + 0x30)
#define UART_IMSC (UARTBASE + 0x38)
#define UART_ICR  (UARTBASE + 0x44)

char uart_getc(void);

void disable_uart() {
  REG(UART_CR) = 0;
}

void uartintr() {
  REG(UART_ICR) = 1 << 4 | 1 << 5;  /* clear interrupt */
}

static void enable_uartintr() {
  REG(UART_IMSC) = 0;
  REG(UART_IMSC) = 1 << 4 | 1 << 5;  /* enable recieve/transmit intr */
}

void uart_putc(char c) {
  while(REG(UART_FR) & FR_TXFF) {}
  REG(UART_DR) = c;
}

char uart_getc() {
  while(REG(UART_FR) & FR_RXFE) {}
  return REG(UART_DR);
}

void uart_puts(char *s) {
  char c;
  while((c = *s)) {
    uart_putc(c);
    s++;
  }
}

void uart_init() {
  disable_uart();

  new_irq(UART_IRQ, uartintr);

  REG(UART_LCRH) = LCRH_FEN | LCRH_WLEN_8BIT;
  REG(UART_CR) = 0x301;  /* RXE, TXE, UARTEN */

  enable_uartintr();
}
