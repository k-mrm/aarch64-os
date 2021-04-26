#include "mono.h"
#include "printk.h"
#include "proc.h"
#include "string.h"

struct proc *curproc;

/* FIXME: tmp */
pid_t newproc(void (*fn)(void)) {
  static pid_t pid = 1;
  struct proc *p;

  for(int i = 0; i < NPROC; i++) {
    p = &proctable[i];
    if(p->state == UNUSED)
      goto found;
  }

  return -1;

found:
  p->pid = pid++;
  memset(&p->context, 0, sizeof(p->context));

  p->state = RUNNABLE;

  return pid;
}

void yield() {
  curproc->state = RUNNABLE;
  schedule();
}

void schedule() {
  for(int i = 0; i < NPROC; i++) {
    if(p->state == RUNNABLE) {
      ;
    }
  }
}
