#include "usys.h"
#include "ulib.h"

u64 strlen(char *s) {
  u64 i = 0;
  while(*s) {
    i++;
    s++;
  }

  return i;
}

int strcmp(const char *s1, const char *s2) {
  while(*s1) {
    if(*s1 != *s2)
      break;
    s1++;
    s2++;
  }

  return *s1 > *s2? 1 : *s1 == *s2? 0 : -1;
}

void *memset(void *dst, int c, u64 n) {
  char *d = dst;

  while(n-- > 0)
    *d++ = c;

  return dst;
}

void *memcpy(void *dst, const void *src, u64 n) {
  return memmove(dst, src, n);
}

void *memmove(void *dst, const void *src, u64 n) {
  char *d = dst;
  const char *s = src;

  if(s > d) {
    while(n-- > 0)
      *d++ = *s++;
  } else {
    d += n;
    s += n;
    while(n-- > 0)
      *--d = *--s;
  }

  return dst;
}


char *strchr(const char *s, int c) {
  char *p = (char *)s;
  while(*p) {
    if(*p == c) {
      return p;
    }
    p++;
  }

  return NULL;
}

char *strtok(char *s1, const char *s2) {
  static char *save = "";
  if(s1)
    save = s1;
  else
    s1 = save;
  char *s = s1;

  while(*s1) {
    if(strchr(s2, *s1)) {
      *s1++ = 0;
      save = s1;
      return s;
    }
    s1++;
  }

  return NULL;
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

  write(1, cur, strlen(cur));
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

  write(1, cur, strlen(cur));
}

int puts(char *s) {
  int r = write(1, s, strlen(s));
  write(1, "\n", 1);
  return r;
}

void printx(u64 n) {
  uprintiu64(n, 16, true);
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
          write(1, "0x", 2);
          uprintiu64(__builtin_va_arg(ap, u64), 16, false);
          break;
        case 'c':
          tmpc = __builtin_va_arg(ap, int);
          write(1, &tmpc, 1);
          break;
        case 's':
          tmps = __builtin_va_arg(ap, char *);
          write(1, tmps, strlen(tmps));
          break;
        case '%':
          write(1, &c, 1);
          break;
        default:
          tmpc = '%';
          write(1, &tmpc, 1);
          write(1, &c, 1);
          break;
      }
    } else {
      write(1, &c, 1);
    }
  }

  __builtin_va_end(ap);

  return 0;
}
