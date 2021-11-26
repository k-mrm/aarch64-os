#include "ulib.h"
#include "usys.h"
#include "fcntl.h"

int main(void) {
  int fd;
  if((fd = open("README.md", O_RDWR)) < 0)
    exit(1);

  char buf[600];
  memset(buf, 'a', 600);

  int n = write(fd, buf, 600);
  printf("wtest %d\n", n);

  exit(0);
}
