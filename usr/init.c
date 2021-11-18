#include "usys.h"
#include "ulib.h"
#include "cdev.h"
#include "fs.h"
#include "fcntl.h"

int main() {
  char *argv[] = {"sh", 0};

  if(mknod("tty0", S_IFCHR, CDEV_CONSOLE) < 0)
    exit(1);

  int fd = open("tty0", O_RDWR);
  dup(fd);    /* 1 */
  dup(fd);    /* 2 */

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
