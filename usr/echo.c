#include "ulib.h"
#include "usys.h"

int main(int argc, char **argv) {
  for(int i = 1; i < argc; i++)
    printf("%s%c", argv[i], i < argc - 1 ? ' ' : '\n');
  exit(0);
}
