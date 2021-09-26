#include "usys.h"
#include "ulib.h"

/* test */
void proc1() {
  char *s = "proc1\n";
  write(s, strlen(s));
  exit(0);
}

void proc2() {
  char *s = "proc2\n";
  write(s, strlen(s));
  exit(0);
}

void init() {
  char *s = "init\n";
  write(s, strlen(s));
  char buf[128] = {0};

  int pid = fork();
  if(pid == 0) {
    write("> ", 2);
    read(buf, 128);
    write(buf, strlen(buf));
    
    if(strcmp("getpid", buf) == 0) {
      int pid = getpid();
      char c = '0' + pid;
      write(&c, 1);
      write("\n", 1);
      exit(0);
    } else {
      write("unknown\n", 8);
      exit(1);
    }
  } else {
    int status;
    wait(&status);
    char c = '0' + status;
    write(&c, 1);
    write("parent\n", 7);
  }
  exit(0);
}
