#include "usys.h"
#include "ulib.h"

int main(int argc, char **argv) {
  if(argc != 2) {
    puts("mkdir [dirname]");
    exit(1);
  }

  if(mkdir(argv[1]) < 0) {
    puts("mkdir: failed");
    exit(1);
  }

  exit(0);
}
