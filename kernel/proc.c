#include "kernel.h"
#include "aarch64.h"
#include "printk.h"
#include "proc.h"
#include "trap.h"
#include "string.h"
#include "kalloc.h"
#include "mm.h"
#include "memmap.h"
#include "log.h"

struct proc proctable[NPROC];
/* proctable[0] == kernel proc */

struct proc *curproc = NULL;
struct proc kproc;

void trapret(void);

void forkret() {
  trapret();
}

/* FIXME: tmp */
pid_t newproc(u64 ubegin, u64 size, u64 uentry) {
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

  char *kstack = kalloc();

  char *sp = kstack + PAGESIZE;
  sp -= sizeof(struct trapframe);
  p->tf = (struct trapframe *)sp;
  memset(p->tf, 0, sizeof(*p->tf));

  p->pgt = kalloc();    /* new pagetable */
  printk("pgt %p ", p->pgt);
  alloc_userspace(p->pgt, ubegin, size);

  printk("newproc %p %p %p\n", ubegin, size, uentry);

  p->tf->elr = uentry;  /* `eret` jump to elr */
  p->tf->spsr = 0x0;    /* switch EL1 to EL0 */
  p->tf->sp = (u64)USTACKTOP; /* sp_el0 */

  p->kstack = kstack;
  p->context.lr = (u64)forkret;
  p->context.sp = (u64)sp;    /* == p->tf */

  p->state = RUNNABLE;

  return pid;
}

int sys_getpid(void) {
  return curproc->pid;
}

void schedule() {
  for(;;) {
    for(int i = 1; i < NPROC; i++) {
      enable_irq();
      struct proc *p = &proctable[i];

      if(p->state == RUNNABLE) {
        load_userspace(p->pgt);
        p->state = RUNNING;

        curproc = p;
        
        cswitch(&kproc.context, &p->context);

        curproc = NULL;
      }
    }
  }
}

void yield() {
  if(!curproc)
    panic("bad yield");

  curproc->state = RUNNABLE;
  cswitch(&curproc->context, &kproc.context);
}

void exit(int ret) {
  if(!curproc)
    panic("bad exit");

  memset(curproc, 0, sizeof(*curproc));
  curproc->state = UNUSED;
  cswitch(&curproc->context, &kproc.context);
}

void curproc_dump() {
  ;
}

/*
static void init_firstproc() {
  struct proc *first = &proctable[1];
  first->pid = 1;
  memset(&first->context, 0, sizeof(first->context));

  char *kstack = kalloc();

  char *sp = kstack + PAGESIZE;
  sp -= sizeof(struct trapframe);
  first->tf = (struct trapframe *)sp;
  memset(first->tf, 0, sizeof(struct trapframe));

  first->tf->elr = 0x0; // FIXME
  first->tf->spsr = 0x0;  // switch to EL0

  first->kstack = kstack;
  first->context.lr = (u64)forkret;
  first->context.sp = (u64)sp;

  first->state = RUNNABLE;
}
*/

void proc_init() {
  memset(&kproc, 0, sizeof(kproc));
  kproc.state = RUNNABLE;
  proctable[0] = kproc;
  curproc = &kproc;
}

