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

struct proc *newproc() {
  static pid_t pid = 1;
  struct proc *p;

  for(int i = 1; i < NPROC; i++) {
    p = &proctable[i];
    if(p->state == UNUSED)
      goto found;
  }

  return NULL;

found:
  p->state = CREATED;
  p->pid = pid++;
  memset(&p->context, 0, sizeof(p->context));

  p->kstack = kalloc();
  char *sp = p->kstack + PAGESIZE;
  sp -= sizeof(struct trapframe);
  p->tf = (struct trapframe *)sp;
  memset(p->tf, 0, sizeof(*p->tf));

  p->pgt = kalloc();

  p->context.lr = (u64)forkret;
  p->context.sp = (u64)sp;    /* == p->tf */

  return p;
}

void free_proc(struct proc *p) {
  kfree(p->kstack);
  memset(p, 0, sizeof(*p));
  p->state = UNUSED;
}

void userproc_init(u64 ubegin, u64 size, u64 entry) {
  struct proc *p = newproc();

  init_userspace(p->pgt, ubegin, size);
  p->size = size;

  map_ustack(p->pgt);

  p->tf->elr = entry;  /* `eret` jump to elr */
  p->tf->spsr = 0x0;    /* switch EL1 to EL0 */
  p->tf->sp = (u64)USTACKTOP; /* sp_el0 */

  p->state = RUNNABLE;
}

int _getpid(void) {
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
        kinfo("enter proc %d\n", p->pid);
        kinfo("sp %p elr %p\n", p->tf->sp, p->tf->elr);

        curproc = p;
        
        cswitch(&kproc.context, &p->context);

        forget_userspace();

        curproc = NULL;
      }
    }
  }
}

void yield() {
  kinfo("yield\n");

  struct proc *p = curproc;

  if(!p)
    panic("bad yield");

  p->state = RUNNABLE;
  cswitch(&p->context, &kproc.context);
}

int _fork() {
  struct proc *p = curproc;
  struct proc *new = newproc();
  if(new == NULL)
    goto err;

  cp_userspace(new->pgt, p->pgt);
  map_ustack(new->pgt);
  new->size = p->size;

  *new->tf = *p->tf;

  new->tf->x0 = 0;

  new->parent = p;
  new->state = RUNNABLE;

  return new->pid;

err:
  return -1;
}

int _wait(int *status) {
  struct proc *p = curproc;

  for(;;) {
    for(int i = 1; i < NPROC; i++) {
      struct proc *cp = &proctable[i];
      if(cp->parent != p)
        continue;

      if(cp->state == ZOMBIE) {
        int pid = cp->pid;
        if(status != NULL)
          *status = cp->ret;

        free_proc(cp);

        return pid;
      }
    }
  }
}

void _exit(int ret) {
  kinfo("exit\n");
  struct proc *p = curproc;

  if(!p)
    panic("bad exit");

  free_userspace(p->pgt, p->size);
  // kfree(p->kstack); /* BUG ON */

  p->ret = ret;
  p->state = ZOMBIE;

  cswitch(&p->context, &kproc.context);
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

