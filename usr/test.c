#include "usys.h"
#include "ulib.h"

/* test */
void proc1() {
  char *s = "proc1\n";
  char a[64] = {[62] = 'a'};
  char b = a[66];
  for(;;) {
    write(&b, 1);
    //write(s, strlen(s));
  }
}

void proc2() {
  char *s = "proc2\n";
  for(;;) {
    write(s, strlen(s));
  }
}
