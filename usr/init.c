#include "usys.h"
#include "ulib.h"

int main() {
  char *argv[] = {"sh", 0};

  write("init\n", 5);
  write("starting sh\n", 12);

  int pid = fork();
  if(pid == 0) {
    exec("sh", (char **)argv);
  } else {
    int status;
    wait(&status);
  }
  exit(0);
}
