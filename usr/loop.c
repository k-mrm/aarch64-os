#include "ulib.h"
#include "usys.h"
#include "fcntl.h"

static void put64(int fd, u64 num) {
  int base = 16;
  char buf[sizeof(num) * 8 + 1] = {0};
  char *end = buf + sizeof(buf);
  char *cur = end - 1;

  do {
    *--cur = "0123456789abcdef"[num % base];
  } while(num /= base);

  write(fd, cur, strlen(cur));
}

int main(void) {
  unsigned long i = 0;
  int fd = open("README.md", O_RDWR);
  for(;;) {
    // printf("loop%d", i++);
    i++;
  }

  exit(0);
}
