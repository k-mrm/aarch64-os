#include "aarch64-os.h"

/* test */
void proc1() {
  u64 s = 0;
  for(;;) {
    printk("proc1: %p %d\n", s++, 1);
  }
}

void proc2() {
  u64 s = 0;
  for(;;) {
    printk("proc2: %p %d\n", s++, 2);
  }
}

void proc3() {
  u64 s = 0;
  for(;;) {
    printk("proc3: %p %d\n", s++, 3);
  }
}
