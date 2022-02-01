#include "usys.h"
#include "ulib.h"

int atou(char *s) {
  int a = 0;
  char c;
  while((c = *s++)) {
    if('0' <= c && c <= '9')
      a = a * 10 + (c - '0');
    else
      return -1;
  }

  return a;
}

int main(int argc, char **argv) {
  if(argc < 2) {
    printf("kill [pid]\n");
    exit(1);
  }

  int pid = atou(argv[1]);
  if(pid < 0)
    exit(1);

  printf("kill %d...\n", pid);
  if(kill(pid) < 0)
    exit(1);

  exit(0);
}
