#include "uni.h"
#include "arm.h"

int cur_el() {
  int el;
  asm volatile("mrs %[e], CurrentEL" : [e]"=r"(el));

  return el >> 2;
}
