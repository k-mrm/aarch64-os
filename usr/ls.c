#include "ulib.h"
#include "usys.h"
#include "stat.h"

void ls(char *path) {
  int fd = open(path, 0);
  if(fd < 0) {
    puts("No such file or directory");
    return;
  }

  struct stat st;
  fstat(fd, &st);

  if(st.st_mode & S_IFREG) {
    puts(path);
  }

  close(fd);
}

int main(int argc, char **argv) {
  if(argc == 1) {
    ls(".");
  } else {
    for(int i = 1; i < argc; i++) {
      ls(argv[i]);
    }
  }
  exit(0);
}
