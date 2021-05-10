#include "uart.h"
#include "gpio.h"
#include "printk.h"

void disable_uart(int n) {
  REG(UART_CR(n)) = 0;
}

static void set_uart_baudrate(int n, u64 baud) {
  u64 bauddiv = (UART_FREQ * 1000) / (16 * baud);
  u64 ibrd = bauddiv / 1000;
  u64 fbrd = ((bauddiv - ibrd * 1000) * 64 + 500) / 1000;

  REG(UART_IBRD(n)) = (u32)ibrd;
  REG(UART_FBRD(n)) = (u32)fbrd;
}

void uart_init(int n) {
  disable_uart(n);
  set_uart_baudrate(n, UART_BAUD);

  set_pinmode(14, ALT0);
  set_pinmode(15, ALT0);

  REG(UART_LCRH(n)) = 0x60;
  REG(UART_CR(n)) = 0x301;  /* RXE, TXE, UARTEN */
}

static bool uart_txff(int n) {
  return REG(UART_FR(n)) & (1 << 5);
}

void uart_putc(int n, char c) {
  while(uart_txff(n)) {}
  REG(UART_DR(n)) = c;
}

void uart_puts(int n, char *s) {
  char c;
  while((c = *s)) {
    while(uart_txff(n)) {}
    REG(UART_DR(n)) = c;
    s++;
  }
}
