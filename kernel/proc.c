#include "mono.h"
#include "printk.h"
#include "proc.h"
#include "string.h"

struct proc proctable[NPROC];
/* proctable[0] == kernel proc */

struct proc *curproc;
struct proc kproc;

void proc_init() {
  memset(&kproc, 0, sizeof(kproc));
  kproc.state = RUNNING;
  proctable[0] = kproc;
}

/* FIXME: tmp */
pid_t newproc(void (*fn)(void)) {
  static pid_t pid = 1;
  struct proc *p;

  for(int i = 1; i < NPROC; i++) {
    p = &proctable[i];
    if(p->state == UNUSED)
      goto found;
  }

  return -1;

found:
  p->pid = pid++;
  memset(&p->context, 0, sizeof(p->context));

  p->context.x9 = (u64)fn;
  p->context.lr = (u64)forkret;

  p->state = RUNNABLE;

  return pid;
}

/* tmp */
void schedule() {
  for(;;) {
    for(int i = 1; i < NPROC; i++) {
      struct proc *p = &proctable[i];
      if(p->state == RUNNABLE) {
        p->state = RUNNING;
        curproc = p;
        swtch(&kproc.context, &p->context);
      }
    }
  }

}

void swtch_sched() {
  swtch(&curproc->context, &kproc.context);
}

void yield() {
  curproc->state = RUNNABLE;
  swtch_sched();
}

