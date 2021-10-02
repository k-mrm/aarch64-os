#include "usys.h"
#include "ulib.h"

int main() {
  char *s = "init\n";
  write(s, strlen(s));
  char buf[128];

  int pid = fork();
  if(pid == 0) {
    exec("sh", NULL);
  } else {
    int status;
    wait(&status);
  }
  exit(0);
}
