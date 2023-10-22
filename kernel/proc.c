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
#include "fs.h"
#include "spinlock.h"

/* for debug */
#define dump_caller() \
  do {  \
    register void *x30 asm("x30");  \
    void *a = x30;  /* save x30 */  \
    printk("%s caller %p\n", __func__, a);  \
  } while(0);

void cswitch(struct context *old, struct context *new);
int fork(void);
static void exitproc(struct proc *p, int ret);

static struct pidallocator {
  struct spinlock lk;
  pid_t pid;
} pidalloc;

static struct proctable {
  struct spinlock lk;
  struct proc procs[NPROC];
} proctable;

static struct cpu cpus[NCPU];

const struct proc *initproc;    // alias &proctable.procs[0]

void trapret(void);

static pid_t newpid() {
  acquire(&pidalloc.lk);
  pid_t pid = pidalloc.pid++;
  release(&pidalloc.lk);

  return pid;
}

struct cpu *mycpu() {
  return &cpus[cpuid()];
}

struct proc *myproc() {
  return mycpu()->proc;
}

static struct proc *pget(pid_t pid) {
  struct proc *p;
    
  acquire(&proctable.lk);

  for(int i = 0; i < NPROC; i++) {
    p = &proctable.procs[i];
    if(p->pid == pid)
      goto found;
  }
  /* not found */
  p = NULL;

found:
  release(&proctable.lk);
  return p;
}

void forkret(void);

static struct proc *newproc() {
  struct proc *p;

  acquire(&proctable.lk);

  for(int i = 0; i < NPROC; i++) {
    p = &proctable.procs[i];
    if(p->state == UNUSED)
      goto found;
  }

  release(&proctable.lk);
  return NULL;

found:
  p->state = CREATED;

  release(&proctable.lk);

  p->pid = newpid();
  kinfo("newproc pid%d\n", p->pid);

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

  p->context.x0 = (u64)p;
  p->context.lr = (u64)forkret;
  p->context.sp = (u64)sp;    /* == p->tf */

  return p;
}

/* already hold proctable.lk */
static void freeproc(struct proc *p) {
  if(!holding(&proctable.lk))
    panic("freeproc");
  kfree(p->kstack);
  memset(p, 0, sizeof(*p));
  p->state = UNUSED;
}

extern char _binary_usr_initcode_start[];
extern u64 _binary_usr_initcode_size[];

void userproc_init() {
  struct proc *p = newproc();
  initproc = p;
  if(!p)
    panic("initproc");

  char *ustart = _binary_usr_initcode_start;
  u64 size = (u64)_binary_usr_initcode_size;
  init_userspace(p->pgt, 0x1000, ustart, size);
  p->size = size;

  map_ustack(p->pgt);

  strcpy(p->name, "proc0");

  p->tf->elr = 0x1000;  /* `eret` jump to elr */
  p->tf->spsr = 0x0;    /* switch EL1 to EL0 */
  p->tf->sp = (u64)USTACKTOP; /* sp_el0 */

  acquire(&proctable.lk);
  p->state = RUNNABLE;
  release(&proctable.lk);
}

int getpid(void) {
  return myproc()->pid;
}

void schedule_tail(struct proc *p) {
  static int firstcall = 1;

  release(&proctable.lk);

  if(firstcall) {
    fs_init();
    firstcall = 0;
  }

  /* path2inode() must be called after fs_init() */
  if(!p->cwd) {
    p->cwd = path2inode("/");
    if(!p->cwd)
      panic("no filesystem");
  }
}

/* running per cpu */
void schedule() {
  struct cpu *cpu = mycpu();
  kinfo("scheduler %d\n", cpuid());

  for(;;) {
    enable_irq();
    acquire(&proctable.lk);

    for(int i = 0; i < NPROC; i++) {
      struct proc *p = &proctable.procs[i];
      if(p->state != RUNNABLE)
        continue;

      kinfo("************* next proc: %d\n", p->pid);

      p->state = RUNNING;
      cpu->proc = p;

      load_userspace(p->pgt);

      kinfo("load userspace %p\n", p->pgt);
      kinfo("******enter proc %d %s\n", p->pid, p->name);
      kinfo("tf->sp %p tf->elr %p\n", p->tf->sp, p->tf->elr);
      kinfo("jump to %p\n", p->context.lr);

      cswitch(&cpu->scheduler, &p->context);

      forget_userspace();

      cpu->proc = NULL;
    }

    release(&proctable.lk);
  }
}

void yield() {
  struct proc *p = myproc();

  acquire(&proctable.lk);

  p->state = RUNNABLE;
  cswitch(&p->context, &mycpu()->scheduler);

  release(&proctable.lk);
}

int clone(void *fn, void *stack) {
  struct proc *p = myproc();
  struct proc *new = newproc();
  if(!new)
    goto fail;

  new->pgt = p->pgt;
  new->size = p->size;

  *new->tf = *p->tf;
  new->tf->elr = (u64)fn;
  new->tf->sp = (u64)stack + PAGESIZE;

  kinfo("clone %p %p\n", fn, new->tf->sp);

  new->cwd = p->cwd;

  for(int fd = 0; fd < NOFILE; fd++) {
    if(p->ofile[fd])
      new->ofile[fd] = dup_file(p->ofile[fd]);
  }

  strcpy(new->name, p->name);

  acquire(&proctable.lk);
  new->state = RUNNABLE;
  new->parent = p;
  new->th = 1;
  release(&proctable.lk);

  return new->pid;

fail:
  return -1;
}

int fork() {
  struct proc *p = myproc();
  struct proc *new = newproc();
  if(!new)
    goto fail;

  if(cp_userspace(new->pgt, p->pgt, 0x1000, p->size) < 0)
    goto fail;

  new->size = p->size;

  *new->tf = *p->tf;
  new->tf->x0 = 0;

  new->cwd = p->cwd;

  for(int fd = 0; fd < NOFILE; fd++) {
    if(p->ofile[fd])
      new->ofile[fd] = dup_file(p->ofile[fd]);
  }

  strcpy(new->name, p->name);

  acquire(&proctable.lk);
  new->state = RUNNABLE;
  new->parent = p;
  release(&proctable.lk);

  return new->pid;

fail:
  if(new) {
    acquire(&proctable.lk);
    freeproc(new);
    release(&proctable.lk);
  }
  return -1;
}

int killproc(struct proc *p, int sig) {
  if(!p)
    return -1;

  kinfo("kill proc: %d %d\n", p->pid, sig);

  p->sig = sig;

  acquire(&proctable.lk);
  if(p->state == SLEEPING)  /* wakeup p */
    p->state = RUNNABLE;
  release(&proctable.lk);

  return 0;
}

int kill(int pid, int sig) {
  return killproc(pget(pid), sig);
}

void sigcheck(struct proc *p) {
  if(p->sig) {
    p->sig = 0;
    kinfo("sig check! %d\n", p->pid);
    exitproc(p, 1);
  }
}

int exec(char *path, char **argv) {
  struct ehdr eh;
  struct phdr ph;
  u64 memsize = 0;
  u64 *pgt = NULL;
  struct inode *ino = path2inode(path);

  kinfo("******exec %s %p %p\n", path, path, argv);

  if(!ino)
    goto fail;

  if(read_inode(ino, (char *)&eh, 0, sizeof(eh)) != sizeof(eh))
    goto fail;
  if(!is_elf(&eh))
    goto fail;
  if(eh.e_type != ET_EXEC)
    goto fail;

  pgt = kalloc();
  if(!pgt)
    goto fail;

  u64 off = eh.e_phoff;
  for(int i = 0; i < eh.e_phnum; i++, off += sizeof(ph)) {
    if(read_inode(ino, (char *)&ph, off, sizeof(ph)) != sizeof(ph))
      goto fail;
    if(ph.p_type != PT_LOAD)
      continue;

    kinfo("exec memsz: %d filesz: %d", ph.p_memsz, ph.p_filesz);

    int sz;
    if((sz = alloc_userspace(pgt, ph.p_vaddr, ph.p_memsz)) < 0)
      goto fail;
    memsize += sz;
    if(load_prg(pgt, ph.p_vaddr, ino, ph.p_offset, ph.p_filesz) < 0)
      goto fail;
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
      goto fail;
    if(sp < stackbase)
      goto fail;

    memcpy(sp, argv[argc], strlen(argv[argc]));
    ustack[argc] = USTACKTOP - (top - sp);
  }

  ustack[argc] = 0;
  sp -= sizeof(ustack[0]) * (argc + 1);
  sp = (char *)((u64)sp & ~(0xf));
  if(sp < stackbase)
    goto fail;
  memcpy(sp, ustack, sizeof(ustack[0]) * (argc + 1));

  struct proc *p = myproc();

  free_userspace(p->pgt, p->size);

  p->size = memsize;
  p->tf->x1 = (u64)USTACKTOP - (top - sp);
  p->tf->elr = eh.e_entry;  /* `eret` jump to elr */
  p->tf->spsr = 0x0;    /* switch EL1 to EL0 */
  p->tf->sp = (u64)USTACKTOP - (top - sp); /* sp_el0 */
  p->pgt = pgt;

  load_userspace(p->pgt);

  return argc;  /* p->tf->x0 */

fail:
  if(pgt)
    kfree(pgt);
  printk("kernel exec failed\n");
  return -1;
}

void sleep(void *chan, struct spinlock *lk) {
  struct proc *p = myproc();

  kinfo("sleep %d\n", p->pid);

  if(lk != &proctable.lk) {
    acquire(&proctable.lk);
    release(lk);
  }

  p->chan = chan;
  p->state = SLEEPING;
  cswitch(&p->context, &mycpu()->scheduler);

  p->chan = NULL;

  release(&proctable.lk);

  if(lk != &proctable.lk)
    acquire(lk);

  sigcheck(p);
}

int waitpid(int pid, int *status) {
  struct proc *p = myproc();

  for(;;) {
    acquire(&proctable.lk);

    for(int i = 0; i < NPROC; i++) {
      struct proc *cp = &proctable.procs[i];
      if(cp->pid != pid)
        continue;

      if(cp->state == ZOMBIE) {
        int ret = cp->pid;
        if(status && copyout(p, status, &cp->ret, sizeof(*status)) < 0)
          ret = -1;

        freeproc(cp);

        release(&proctable.lk);

        return ret;
      }
    }

    sleep(p, &proctable.lk);
  }
}

int wait(int *status) {
  struct proc *p = myproc();

  for(;;) {
    acquire(&proctable.lk);

    for(int i = 0; i < NPROC; i++) {
      struct proc *cp = &proctable.procs[i];
      if(cp->parent != p)
        continue;

      if(cp->state == ZOMBIE) {
        int ret = cp->pid;
        if(status && copyout(p, status, &cp->ret, sizeof(*status)) < 0)
          ret = -1;

        freeproc(cp);

        release(&proctable.lk);

        return ret;
      }
    }

    sleep(p, &proctable.lk);
  }
}

static void wakeup_acquired(void *chan) {
  for(int i = 0; i < NPROC; i++) {
    struct proc *p = &proctable.procs[i];
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
  }
}

void wakeup(void *chan) {
  acquire(&proctable.lk);
  wakeup_acquired(chan);
  release(&proctable.lk);
}

void reparent(struct proc *pp) {
  for(int i = 0; i < NPROC; i++) {
    struct proc *p = &proctable.procs[i];
    if(p->parent == pp)
      p->parent = (struct proc *)initproc;
  }
}

static void exitproc(struct proc *p, int ret) {
  if(!p)
    return;
  if(p->pid == 0)
    panic("init exit");

  if(!p->th)
    free_userspace(p->pgt, p->size);

  for(int i = 0; i < NOFILE; i++)
    close(i);

  p->ret = ret;

  acquire(&proctable.lk);

  reparent(p);

  wakeup_acquired(p->parent);

  p->state = ZOMBIE;

  cswitch(&p->context, &mycpu()->scheduler);

  panic("unreachable");
}

void exit(int ret) {
  kinfo("exit\n");

  exitproc(myproc(), ret);
}

int chdir(char *path) {
  struct proc *p = myproc();
  struct inode *ino = path2inode(path);
  if(!ino)
    return -1;
  if(!S_ISDIR(ino->mode))
    return -1;

  p->cwd = ino;

  return 0;
}

void dumpps() {
  static const char *pstatemap[] = {
    [CREATED]   "created",
    [RUNNING]   "running",
    [RUNNABLE]  "runnable",
    [SLEEPING]  "sleeping",
    [ZOMBIE]    "zombie",
  };

  acquire(&proctable.lk);

  for(int i = 0; i < NPROC; i++) {
    struct proc *p = &proctable.procs[i];
    if(p->state != UNUSED)
      printk("%s %d %s\n", pstatemap[p->state], p->pid, p->name);
    if(p->state == SLEEPING)
      printk("chan %p\n", p->chan);
  }

  for(int i = 0; i < NCPU; i++) {
    struct proc *p = cpus[i].proc;
    if(p)
      printk("cpu%d %s %d %s\n", i, pstatemap[p->state], p->pid, p->name);
    else
      printk("cpu%d no proc\n", i);
  }

  release(&proctable.lk);
}

void *sbrk(int incr) {
  struct proc *p = myproc();
  void *h;

  if(incr > 0) {
    h = grow_userspace(p->pgt, 0x1000, p->size, p->size + incr);
  } else {
    /* TODO: shrink heap */
    h = NULL;
  }

  p->size += incr;

  return h;
}

void dump_kstack(struct proc *p) {
  for(int i = 0; i < PAGESIZE; i++) {
    printk("%x ", p->kstack[i]);
  }
}

static int copyio_fault(struct proc *p) {
  p->fault_handler = NULL;
  return -1;
}

int copyout(struct proc *p, void *udst, const void *src, u64 sz) {
  /* do not call function: avoiding rewrite x30 */
  p->fault_handler = copyio_fault;

  char *d = udst;
  const char *s = src;

  if(s > d) {
    while(sz-- > 0)
      *d++ = *s++;
  } else {
    d += sz;
    s += sz;
    while(sz-- > 0)
      *--d = *--s;
  }

  p->fault_handler = NULL;

  return 0;
}

int copyin(struct proc *p, void *dst, const void *usrc, u64 sz) {
  /* do not call function: avoiding rewrite x30 */
  p->fault_handler = copyio_fault;

  char *d = dst;
  const char *s = usrc;

  if(s > d) {
    while(sz-- > 0)
      *d++ = *s++;
  } else {
    d += sz;
    s += sz;
    while(sz-- > 0)
      *--d = *--s;
  }

  p->fault_handler = NULL;

  return 0;
}

void proc_init() {
  memset(cpus, 0, sizeof(cpus));

  lock_init(&pidalloc.lk);
  pidalloc.pid = 0;

  lock_init(&proctable.lk);
  memset(&proctable.procs, 0, sizeof(proctable.procs));
}
