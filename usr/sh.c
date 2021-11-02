#include "ulib.h"
#include "usys.h"

int parse(char *cmd, char **argv) {
  static const char *ws = " \n\t";
  while(strchr(ws, *cmd))
    cmd++;

  int argc = 0;
  argv[argc++] = strtok(cmd, ws);
  char *tok;
  while((tok = strtok(NULL, ws))) {
    argv[argc++] = tok;
  }
  argv[argc] = 0;

  if(argv[0] == NULL)
    return -1;

  return argc;
}

void read_cmd(char *buf) {
  memset(buf, 0, 128);
  printf("$ ");
  read(0, buf, 128);
}

int builtincmd(int argc, char **argv) {
  if(strcmp(argv[0], "cd") == 0) {
    if(argc < 2) {
      if(chdir("/") < 0)
        return -1;
    } else if(argc == 2) {
      if(chdir(argv[1]) < 0) {
        puts("cd: failed");
        return -1;
      }
    } else {
      puts("too many args");
      return -1;
    }

    return 1;
  } else if(strcmp(argv[0], "exit") == 0) {
    exit(0);
    return 1;
  }

  return 0;
}

int main(void) {
  char *argv[8];
  char buf[128] = {0};
  for(;;) {
    read_cmd(buf);
    int argc = parse(buf, argv);
    if(argc < 0)
      continue;

    if(builtincmd(argc, argv))
      continue;

    int pid = fork();
    if(pid == 0) {
      exec(argv[0], argv);
      printf("%s failed\n", argv[0]);
      exit(1);
    } else {
      int status;
      wait(&status);
    }
  }
}
