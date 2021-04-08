#include "proc.h"

static pid_t procid() {
  static pid_t p = 0;
  return p++;
}
