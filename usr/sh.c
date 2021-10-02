#include "ulib.h"
#include "usys.h"

int main(void) {
  char buf[128] = {0};
  int pid = fork();
  for(;;) {
    if(pid == 0) {
      write("$ ", 2);
      read(buf, 128);
      exec(buf, NULL);
      memset(buf, 0, 128);
    } else {
      int status;
      wait(&status);
      char c = '0' + status;
      write(&c, 1);
      write("\n", 1);
    }
  }
}
