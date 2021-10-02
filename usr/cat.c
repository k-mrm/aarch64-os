#include "usys.h"

char buf[512];

void cat(int fd) {
  int n;

  while((n = read(buf, sizeof(buf))) > 0) {
    if(write(buf, n) != n) {
      exit(1);
    }
  }
}

int main(void) {
  cat(0);
  exit(0);
}
