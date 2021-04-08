#include "string.h"
#include "mono.h"

void *memcpy(void *dst, const void *src, u64 n) {
  return memmove(dst, src, n);
}

void *memmove(void *dst, const void *src, u64 n) {
  char *d = dst;
  const char *s = src;

  if(s > d) {
    while(n-- > 0)
      *d++ = *s++;
  }
  else {
    d += n;
    s += n;
    while(n-- > 0)
      *--d = *--s;
  }

  return dst;
}

void *memset(void *dst, int c, u64 n) {
  char *d = dst;

  while(n-- > 0)
    *d++ = c;

  return dst;
}
