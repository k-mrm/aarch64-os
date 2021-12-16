#include "kernel.h"
#include "aarch64.h"
#include "printk.h"
#include "console.h"
#include "spinlock.h"

struct spinlock printk_lk;

#define va_list __builtin_va_list
#define va_start(v, l)  __builtin_va_start(v, l)
#define va_arg(v, l)  __builtin_va_arg(v, l)
#define va_end(v) __builtin_va_end(v)
#define va_copy(d, s) __builtin_va_copy(d, s)

static void printiu32(i32 num, int base, bool sign) {
  char buf[sizeof(num) * 8 + 1] = {0};
  char *end = buf + sizeof(buf);
  char *cur = end - 1;
  u32 unum;
  bool neg = false;

  if(sign && num < 0) {
    unum = (u32)(-(num + 1)) + 1;
    neg = true;
  } else {
    unum = (u32)num;
  }

  do {
    *--cur = "0123456789abcdef"[unum % base];
  } while(unum /= base);

  if(neg)
    *--cur = '-';

  while(*cur)
    csputc(&cons1, *cur++);
}

static void printiu64(i64 num, int base, bool sign) {
  char buf[sizeof(num) * 8 + 1] = {0};
  char *end = buf + sizeof(buf);
  char *cur = end - 1;
  u64 unum;
  bool neg = false;

  if(sign && num < 0) {
    unum = (u64)(-(num + 1)) + 1;
    neg = true;
  } else {
    unum = (u64)num;
  }

  do {
    *--cur = "0123456789abcdef"[unum % base];
  } while(unum /= base);

  if(neg)
    *--cur = '-';

  while(*cur)
    csputc(&cons1, *cur++);
}

static int vprintk(const char *fmt, va_list ap) {
  acquire(&printk_lk);

  csputc(&cons1, '0'+cpuid());

  for(int i = 0; fmt[i]; i++) {
    char c = fmt[i];
    if(c == '%') {
      c = fmt[++i];

      switch(c) {
        case 'd':
          printiu32(va_arg(ap, i32), 10, true);
          break;
        case 'u':
          printiu32(va_arg(ap, u32), 10, false);
          break;
        case 'x':
          printiu64(va_arg(ap, u64), 16, false);
          break;
        case 'p':
          csputc(&cons1, '0');
          csputc(&cons1, 'x');
          printiu64(va_arg(ap, u64), 16, false);
          break;
        case 'c':
          csputc(&cons1, va_arg(ap, int));
          break;
        case 's': {
          char *s = va_arg(ap, char *);
          if(s == NULL)
            s = "(null)";

          while(*s)
            csputc(&cons1, *s++);
          break;
        }
        case '%':
          csputc(&cons1, '%');
          break;
        default:
          csputc(&cons1, '%');
          csputc(&cons1, c);
          break;
      }
    } else {
      csputc(&cons1, c);
    }
  }

  release(&printk_lk);

  return 0;
}

int printk(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  vprintk(fmt, ap);

  va_end(ap);

  return 0;
}

void panic(const char *s, ...) {
  va_list ap;
  va_start(ap, s);

  printk("[panic]: ");
  vprintk(s, ap);
  printk("\n");

  va_end(ap);

  disable_irq();

  for(;;)
    wfe();
}

void printk_init(void) {
  lock_init(&printk_lk);
}
