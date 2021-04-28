#include "uart.h"
#include "gpio.h"

void disable_uart(int n) {
  REG(UART_CR(n)) = 0;
}

void set_uart_baudrate(int n, u32 baud) {
  u32 bauddiv = (1000 * (u32)UART_FREQ) / (16 * baud);
  u32 ibrd = bauddiv / 1000;
  u32 fbrd = ((bauddiv - ibrd * 1000) * 64 + 500) / 1000;

  REG(UART_IBRD(n)) = ibrd;
  REG(UART_FBRD(n)) = fbrd;
}

void uart_init(int n) {
  disable_uart(n);
  set_uart_baudrate(n, UART_BAUD);

  gpio_clr(14);
  gpio_clr(15);
  set_pinmode(14, ALT0);
  set_pinmode(15, ALT0);

  REG(UART_LCRH(n)) = 0x70;
  REG(UART_CR(n)) = 0x301;  /* RXE, TXE, UARTEN */
}

void uart_putc(int n, char c) {
  REG(UART_DR(n)) = c;
}

void uart_puts(int n, char *s) {
  char c;
  while((c = *s)) {
    REG(UART_DR(n)) = c;
    s++;
  }
}
