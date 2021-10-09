#include "usys.h"
#include "ulib.h"

int main() {
  char *argv[] = {"sh", 0};

  write(1, "init\n", 5);
  write(1, "starting sh\n", 12);

  int pid = fork();
  if(pid == 0) {
    exec("sh", argv);
  } else {
    int status;
    wait(&status);
  }
  exit(0);
}
