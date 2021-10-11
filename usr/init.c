#include "usys.h"
#include "ulib.h"

int main() {
  char *argv[] = {"sh", 0};

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
