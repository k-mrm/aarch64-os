#ifndef AARCH64_OS_CONSOLE_H
#define AARCH64_OS_CONSOLE_H

#include "kernel.h"
#include "spinlock.h"

#define USE_UART

struct console {
  struct spinlock lk;
  char *readbuf;
  int bufc;
  u64 bufsz;
  int uartid;
};

extern struct console cons1;

void console_init(void);
void csputc(struct console *cs, char c);
void consoleintr(struct console *cs, int c);

#endif
