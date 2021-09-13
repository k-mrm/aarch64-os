#include "usys.h"
#include "ulib.h"

u64 strlen(char *s) {
  int i = 0;
  while(*s) {
    i++;
    s++;
  }

  return i;
}

static void uprintiu32(i32 num, int base, bool sign) {
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

  write(cur, strlen(cur));
}

static void uprintiu64(i64 num, int base, bool sign) {
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

  write(cur, strlen(cur));
}

int printf(const char *fmt, ...) {
  char tmpc;
  char *tmps;

  __builtin_va_list ap;
  __builtin_va_start(ap, fmt);

  for(int i = 0; fmt[i]; i++) {
    char c = fmt[i];
    if(c == '%') {
      c = fmt[++i];

      switch(c) {
        case 'd':
          uprintiu32(__builtin_va_arg(ap, i32), 10, true);
          break;
        case 'u':
          uprintiu32(__builtin_va_arg(ap, u32), 10, false);
          break;
        case 'x':
          uprintiu64(__builtin_va_arg(ap, u64), 16, false);
          break;
        case 'p':
          write("0x", 2);
          uprintiu64(__builtin_va_arg(ap, u64), 16, false);
          break;
        case 'c':
          tmpc = __builtin_va_arg(ap, int);
          write(&tmpc, 1);
          break;
        case 's':
          tmps = __builtin_va_arg(ap, char *);
          write(tmps, strlen(tmps));
          break;
        case '%':
          write(&c, 1);
          break;
        default:
          tmpc = '%';
          write(&tmpc, 1);
          write(&c, 1);
          break;
      }
    } else {
      write(&c, 1);
    }
  }

  __builtin_va_end(ap);

  return 0;
}
