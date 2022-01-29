#include "string.h"
#include "kernel.h"
#include "font.h"
#include "console.h"
#include "driver/uart.h"
#include "file.h"
#include "cdev.h"
#include "proc.h"
#include "mm.h"
#include "log.h"
#include "kalloc.h"

#define BACKSPACE 127
#define C(x)  ((x)-'@')

struct console cons1;

void csputc(struct console *cs, char c) {
  if(c == BACKSPACE) {
    uart_putc('\b');
    uart_putc(' ');
    uart_putc('\b');
  } else {
    uart_putc(c);
  }
}

static int cswrite(struct console *cs, char *s, u64 size) {
  u64 i;
  for(i = 0; i < size; i++) {
    uart_putc(*s++);
  }

  return i;
}

static int csread(struct console *cs, char *buf, u64 size) {
  acquire(&cs->lk);

  cs->readbuf = (char *)uva2ka(myproc()->pgt, (u64)buf);
  cs->bufc = 0;
  cs->bufsz = size;

  sleep(cs->readbuf, &cs->lk);

  u64 sz = cs->bufc;

  cs->readbuf = NULL;
  cs->bufc = 0;
  cs->bufsz = 0;

  release(&cs->lk);

  return sz;
}

void consoleintr(struct console *cs, int c) {
  acquire(&cs->lk);
  kinfo("consoleintr!\n");

  if(!c)
    goto end;

  switch(c) {
    case C('P'):
      dumpps();
      printk("%d KB free\n", freed_mem_kb());
      goto end;
  }

  if(!cs->readbuf)
    goto end;

  c = (c == '\r')? '\n' : c;

  switch(c) {
    case BACKSPACE:
      if(cs->bufc > 0) {
        cs->bufc--;
        csputc(cs, c);  /* echo back */
      }
      break;
    case C('D'):
      break;
    default:
      cs->readbuf[cs->bufc++] = (char)c;
      csputc(cs, c);  /* echo back */
      break;
  }


  if(c == '\n' || c == C('D') || cs->bufc == cs->bufsz)
    wakeup(cs->readbuf);

end:
  release(&cs->lk);
}

static int console_write(struct file *f, char *buf, u64 size) {
  return cswrite(&cons1, buf, size);
}

static int console_read(struct file *f, char *buf, u64 size) {
  return csread(&cons1, buf, size);
}

struct cdevsw cons_devsw = {
  .read = console_read,
  .write = console_write,
};

void console_init() {
  uart_init();

  lock_init(&cons1.lk);

  register_cdev(CDEV_CONSOLE, &cons_devsw);
}
