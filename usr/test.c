#include "usys.h"
#include "ulib.h"

/* test */
int main() {
  exit(0);
}

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
