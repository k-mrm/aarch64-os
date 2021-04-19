#include "mono.h"
#include "printk.h"
#include "proc.h"

struct proc *curproc;

/* FIXME: tmp */
pid_t newproc(void (*fn)(void)) {
  pid_t pid;
  struct proc *p;

  for(pid = 0; pid < NPROC; pid++) {
    p = &proctable[pid];
    if(p->state == UNUSED)
      goto found;
  }

  return -1;

found:
  p->pid = pid;

  return pid;
}

void schedule() {
  struct proc *cp = curproc;
}
