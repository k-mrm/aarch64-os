#include "ulib.h"
#include "usys.h"

int main(int argc, char **argv) {
  char c = argc + '0';
  write(&c, 1);
  for(int i = 1; i < argc; i++) {
    write(argv[i], strlen(argv[i]));
    write(i < argc-1? " " : "\n", 1);
  }
  exit(0);
}
