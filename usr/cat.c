#include "usys.h"

char buf[512];

void cat(int fd) {
  int n;

  while((n = read(fd, buf, sizeof(buf))) > 0) {
    if(write(1, buf, n) != n) {
      exit(1);
    }
  }
}

int main(int argc, char **argv) {
  if(argc == 1) {
    cat(0);
  } else {
    for(int i = 1; i < argc; i++) {
      int fd;
      char *p = argv[i];
      if((fd = open(p, 0)) < 0)
        exit(1);
      cat(fd);
    }
  }
  exit(0);
}
