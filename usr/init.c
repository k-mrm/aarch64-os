#include "usys.h"
#include "ulib.h"

int main() {
  write("init\n", 5);
  write("starting sh\n", 12);

  int pid = fork();
  if(pid == 0) {
    exec("sh", NULL);
  } else {
    int status;
    wait(&status);
  }
  exit(0);
}
