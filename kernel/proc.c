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
#include "elf.h"
#include "ext2.h"

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
  if(!p->kstack)
    return NULL;
  char *sp = p->kstack + PAGESIZE;
  sp -= sizeof(struct trapframe);
  p->tf = (struct trapframe *)sp;
  memset(p->tf, 0, sizeof(*p->tf));

  p->pgt = kalloc();
  if(!p->pgt)
    return NULL;

  p->context.lr = (u64)forkret;
  p->context.sp = (u64)sp;    /* == p->tf */

  return p;
}

void free_proc(struct proc *p) {
  kfree(p->kstack);
  memset(p, 0, sizeof(*p));
  p->state = UNUSED;
}

extern char _binary_usr_initcode_start[];
extern u64 _binary_usr_initcode_size[];

void userproc_init() {
  struct proc *p = newproc();
  if(!p)
    panic("initproc");

  char *ustart = _binary_usr_initcode_start;
  u64 size = (u64)_binary_usr_initcode_size;
  p->size = init_userspace(p->pgt, ustart, size);

  map_ustack(p->pgt);

  p->cwd = path2inode("/");
  if(!p->cwd)
    panic("no filesystem");

  p->tf->elr = 0x0;  /* `eret` jump to elr */
  p->tf->spsr = 0x0;    /* switch EL1 to EL0 */
  p->tf->sp = (u64)USTACKTOP; /* sp_el0 */

  p->state = RUNNABLE;
}

int getpid(void) {
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
        kinfo("load userspace %p\n", p->pgt);
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

  p->state = RUNNABLE;
  cswitch(&p->context, &kproc.context);
}

int fork() {
  struct proc *p = curproc;
  struct proc *new = newproc();
  if(!new)
    goto err;

  if(cp_userspace(new->pgt, p->pgt, p->size) < 0)
    return -1;

  new->size = p->size;
  *new->tf = *p->tf;

  new->tf->x0 = 0;

  new->cwd = p->cwd;
  new->parent = p;
  new->state = RUNNABLE;

  return new->pid;

err:
  return -1;
}

int exec(char *path, char **argv) {
  kinfo("exec %s\n", path);
  struct inode *ino = path2inode(path);
  if(!ino)
    return -1;

  struct ehdr eh;
  struct phdr ph;
  int memsize = 0;

  if(read_inode(ino, (char *)&eh, 0, sizeof(eh)) != sizeof(eh))
    return -1;
  if(!is_elf(&eh))
    return -1;
  if(eh.e_type != ET_EXEC)
    return -1;

  u64 *pgt = kalloc();
  if(!pgt)
    return -1;

  int i = 0;
  u64 off = eh.e_phoff;
  for(; i < eh.e_phnum; i++, off += sizeof(ph)) {
    if(read_inode(ino, (char *)&ph, off, sizeof(ph)) != sizeof(ph))
      return -1;
    if(ph.p_type != PT_LOAD)
      continue;
    memsize += alloc_userspace(pgt, ph.p_vaddr, ino, ph.p_offset, ph.p_memsz);
  }

  u64 ustack[9];
  char *stackbase = map_ustack(pgt);
  char *sp = stackbase + PAGESIZE;
  char *top = sp;
  int argc = 0;

  for(; argv && argv[argc]; argc++) {
    sp -= strlen(argv[argc]) + 1;
    sp = (char *)((u64)sp & ~(0xf));
    if(argc >= 8)
      return -1;
    if(sp < stackbase)
      return -1;
    memcpy(sp, argv[argc], strlen(argv[argc]));
    ustack[argc] = USTACKTOP - (top - sp);
  }

  ustack[argc] = 0;
  sp -= sizeof(ustack[0]) * (argc + 1);
  sp = (char *)((u64)sp & ~(0xf));
  if(sp < stackbase)
    return -1;
  memcpy(sp, ustack, sizeof(ustack[0]) * (argc + 1));

  struct proc *p = curproc;

  free_userspace(p->pgt, p->size);

  p->size = memsize;
  p->tf->x1 = (u64)USTACKTOP - (top - sp);
  p->tf->elr = eh.e_entry;  /* `eret` jump to elr */
  p->tf->spsr = 0x0;    /* switch EL1 to EL0 */
  p->tf->sp = (u64)USTACKTOP - (top - sp); /* sp_el0 */
  p->pgt = pgt;

  load_userspace(p->pgt);

  return argc;  /* p->tf->x0 */
}

int wait(int *status) {
  struct proc *p = curproc;

  for(;;) {
    for(int i = 1; i < NPROC; i++) {
      struct proc *cp = &proctable[i];
      if(cp->parent != p)
        continue;

      if(cp->state == ZOMBIE) {
        int pid = cp->pid;
        if(status)
          *status = cp->ret;

        free_proc(cp);

        return pid;
      }
    }

    sleep(p);
  }
}

void sleep(struct proc *p) {
  kinfo("sleeep\n");
  p->state = SLEEPING;
  
  cswitch(&p->context, &kproc.context);
}

void wakeup(struct proc *proc) {
  kinfo("wakeup\n");

  for(int i = 1; i < NPROC; i++) {
    struct proc *p = &proctable[i];
    if(p->state == SLEEPING && p == proc)
      p->state = RUNNABLE;
  }
}

void exit(int ret) {
  kinfo("exit\n");
  struct proc *p = curproc;

  free_userspace(p->pgt, p->size);

  p->ret = ret;
  p->state = ZOMBIE;

  wakeup(p->parent);

  cswitch(&p->context, &kproc.context);
}

void dump_proc(struct proc *p) {
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

