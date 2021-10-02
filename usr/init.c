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

      if(strcmp("getpid", buf) == 0) {
        int pid = getpid();
        char c = '0' + pid;
        write(&c, 1);
        write("\n", 1);
      } else if(strcmp("exit", buf) == 0) {
        write("bye\n", 4);
        exit(0);
      } else {
        write("unknown\n", 8);
      }
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
