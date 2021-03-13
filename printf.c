#include "mono.h"
#include "arm.h"
#include "printf.h"
#include "console.h"

static void printiu32(i32 num, int base, bool sign) {
  char buf[sizeof(num) * 8 + 1] = {0};
  char *end = buf + sizeof(buf);
  char *cur = end - 1;
  u32 unum;
  bool neg = false;

  if(sign && num < 0) {
    unum = (u32)(-(num + 1)) + 1;
    neg = true;
  }
  else {
    unum = (u32)num;
  }

  do {
    *--cur = "0123456789abcdef"[unum % base];
  } while(unum /= base);
  if(neg) {
    *--cur = '-';
  }

  csputs(&cons1, cur);
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
  }
  else {
    unum = (u64)num;
  }

  do {
    *--cur = "0123456789abcdef"[unum % base];
  } while(unum /= base);
  if(neg) {
    *--cur = '-';
  }

  csputs(&cons1, cur);
}

int printf(const char *fmt, ...) {
  __builtin_va_list ap;
  __builtin_va_start(ap, fmt);

  for(int i = 0; fmt[i]; i++) {
    char c = fmt[i];
    if(c == '%') {
      c = fmt[++i];

      switch(c) {
        case 'd':
          printiu32(__builtin_va_arg(ap, i32), 10, true);
          break;
        case 'u':
          printiu32(__builtin_va_arg(ap, u32), 10, false);
          break;
        case 'p':
          printiu64(__builtin_va_arg(ap, u64), 16, false);
          break;
        case 'c':
          csputc(&cons1, __builtin_va_arg(ap, int));
          break;
        case 's':
          csputs(&cons1, __builtin_va_arg(ap, char *));
          break;
        case '%':
          csputc(&cons1, '%');
          break;
        default:
          csputc(&cons1, '%');
          csputc(&cons1, c);
          break;
      }
    }
    else {
      csputc(&cons1, c);
    }
  }

  __builtin_va_end(ap);

  return 0;
}

extern void hang(void);

void panic(const char *s) {
  printf("[panic]: %s\n", s);

  for(;;)
    wfe();
}
