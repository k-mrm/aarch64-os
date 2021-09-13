#include "kernel.h"

/* test */
void proc1() {
  u64 s = 0;
  for(;;) {
    write("proc1\n");
  }
}

void proc2() {
  u64 s = 0;
  for(;;) {
    write("proc2\n");
  }
}
