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

  int pid = fork();
  if(pid == 0) {
    write("child\n", 6);
    exit(0);
  } else {
    wait(NULL);
    write("parent\n", 7);
  }
  exit(0);
}
