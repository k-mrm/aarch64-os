#include "mono.h"
#include "printk.h"
#include "proc.h"

static pid_t procid() {
  static pid_t p = 0;
  return p++;
}

struct proc *newproc() {
  pid_t pid = procid();
  printk("pid %d", pid);
}
