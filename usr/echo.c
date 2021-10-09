#include "ulib.h"
#include "usys.h"

int main(int argc, char **argv) {
  for(int i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    write(1, i < argc-1? " " : "\n", 1);
  }
  exit(0);
}
