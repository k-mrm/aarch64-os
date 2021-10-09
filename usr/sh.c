#include "ulib.h"
#include "usys.h"

void parse(char *cmd, char **argv) {
  static const char *ws = " \n\t";
  int argc = 0;
  argv[argc++] = strtok(cmd, ws);
  char *tok;
  while((tok = strtok(NULL, ws))) {
    argv[argc++] = tok;
  }
  argv[argc] = 0;
}

void read_cmd(char *buf) {
  memset(buf, 0, 128);
  write(1, "$ ", 2);
  read(0, buf, 128);
}

int main(void) {
  char *argv[8];
  char buf[128] = {0};
  for(;;) {
    read_cmd(buf);
    parse(buf, argv);

    int pid = fork();
    if(pid == 0) {
      exec(argv[0], argv);
      write(1, "failed\n", 7);
      exit(1);
    } else {
      int status;
      wait(&status);
    }
  }
}
