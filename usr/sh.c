#include "ulib.h"
#include "usys.h"

int main(void) {
  char buf[128] = {0};
  int pid = fork();
  for(;;) {
    if(pid == 0) {
      memset(buf, 0, 128);
      write("$ ", 2);
      read(buf, 128);
      buf[strlen(buf) - 1] = '\0';
      exec(buf, NULL);
      write("failed\n", 7);
    } else {
      int status;
      wait(&status);
    }
  }
}
