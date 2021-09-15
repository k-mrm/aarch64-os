#include "usys.h"
#include "ulib.h"

/* test */
void proc1() {
  char *s = "proc1\n";
  char a[64];
  char b = a[114514];
  for(;;) {
    write(&b, 1);
  }
}

void proc2() {
  char *s = "proc2\n";
  for(;;) {
    write(s, strlen(s));
  }
}
