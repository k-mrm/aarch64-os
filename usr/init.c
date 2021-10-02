#include "usys.h"
#include "ulib.h"

int main() {
  char *s = "init\n";
  write(s, strlen(s));
  char buf[128];

  int pid = fork();
  if(pid == 0) {
    for(;;) {
      for(int i = 0; i < 128; i++)
        buf[i] = 0;
      read(buf, 128);
    }
  } else {
    int status;
    wait(&status);
    char c = '0' + status;
    write(&c, 1);
    write("\n", 1);
  }
  exit(0);
}
