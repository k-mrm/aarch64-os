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
  write("$ ", 2);
  read(buf, 128);
}

int main(void) {
  char *argv[8];
  char buf[128] = {0};
  int pid = fork();
  for(;;) {
    if(pid == 0) {
      read_cmd(buf);
      parse(buf, argv);
      exec(argv[0], argv);
      write("failed\n", 7);
    } else {
      int status;
      wait(&status);
    }
  }
}
