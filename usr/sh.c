#include "ulib.h"
#include "usys.h"

int main(void) {
  char buf[128] = {0};
  int pid = fork();
  if(pid == 0) {
    read(buf, 128);
  } else {
    int status;
    wait(&status);
  }
}
