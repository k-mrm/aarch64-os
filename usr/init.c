#include "usys.h"
#include "ulib.h"
#include "cdev.h"
#include "fs.h"

int main() {
  char *argv[] = {"sh", 0};

  int fd = mknod("tty0", S_IFCHR, CDEV_CONSOLE);
  if(fd < 0)
    exit(0);

  puts("init");
  puts("starting sh");

  int pid = fork();
  if(pid == 0) {
    exec("sh", argv);
  } else {
    int status;
    wait(&status);
  }
  exit(0);
}
