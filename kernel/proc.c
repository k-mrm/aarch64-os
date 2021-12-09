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

static struct pidallocator {
  struct spinlock lk;
  pid_t pid;
} pidalloc;

static struct proctable {
  struct spinlock lk;
  struct proc procs[NPROC];
} proctable;

void trapret(void);

static pid_t newpid() {
  kinfo("newpid\n");
  acquire(&pidalloc.lk);
  pid_t pid = pidalloc.pid++;
  release(&pidalloc.lk);

  return pid;
}

void forkret() {
  release(&proctable.lk);
  trapret();
  /*
   *  FIXME: forkret return to trapret
   */
}

struct cpu *mycpu() {
  return &cpus[cpuid()];
}

struct proc *myproc() {
  return mycpu()->proc;
}

struct proc *newproc() {
  struct proc *p;

  kinfo("newproc a\n");
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

  p->context.lr = (u64)trapret;
  p->context.sp = (u64)sp;    /* == p->tf */

  return p;
}

/* already hold proctable.lk */
void freeproc(struct proc *p) {
  kinfo("freeproc\n");
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

  memcpy(p->name, "proc0", 6);

  p->cwd = path2inode("/");
  if(!p->cwd)
    panic("no filesystem");

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

void schedule() {
  struct cpu *cpu = mycpu();

  for(;;) {
    kinfo("sched a\n");
    acquire(&proctable.lk);
    for(int i = 0; i < NPROC; i++) {
      struct proc *p = &proctable.procs[i];
      if(p->state != RUNNABLE)
        continue;

      p->state = RUNNING;

      kinfo("sched release ptable lock\n");
      release(&proctable.lk);

      load_userspace(p->pgt);

      kinfo("scheduler cpu %d\n", cpuid());
      kinfo("load userspace %p\n", p->pgt);
      kinfo("enter proc %d\n", p->pid);
      kinfo("tf %p\n", p->tf);
      kinfo("tf->sp %p tf->elr %p\n", p->tf->sp, p->tf->elr);
      kinfo("jump to %p\n", p->context.lr);

      cpu->proc = p;

      cswitch(&cpu->scheduler, &p->context);  /* acquire proctable.lk */

      forget_userspace();

      cpu->proc = NULL;
    }
    release(&proctable.lk);
  }
}

void yield() {
  kinfo("yield\n");

  struct proc *p = myproc();

  acquire(&proctable.lk);

  p->state = RUNNABLE;
  cswitch(&p->context, &mycpu()->scheduler);  /* release proctable.lk */
}

int fork() {
  struct proc *p = myproc();
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

  for(int fd = 0; fd < NOFILE; fd++) {
    if(p->ofile[fd])
      new->ofile[fd] = dup_file(p->ofile[fd]);
  }

  return new->pid;

err:
  return -1;
}

int kill(int pid, int sig) {
  ;
}

int exec(char *path, char **argv) {
  kinfo("exec %s %p %p\n", path, path, argv);
  struct inode *ino = path2inode(path);
  if(!ino)
    goto fail;
  kinfo("exec: path2inode\n");

  struct ehdr eh;
  struct phdr ph;
  int memsize = 0;

  if(read_inode(ino, (char *)&eh, 0, sizeof(eh)) != sizeof(eh))
    goto fail;
  if(!is_elf(&eh))
    goto fail;
  if(eh.e_type != ET_EXEC)
    goto fail;

  kinfo("exec: elf check\n");

  u64 *pgt = kalloc();
  if(!pgt)
    return -1;

  int i = 0;
  u64 off = eh.e_phoff;
  for(; i < eh.e_phnum; i++, off += sizeof(ph)) {
    if(read_inode(ino, (char *)&ph, off, sizeof(ph)) != sizeof(ph))
      goto fail;
    if(ph.p_type != PT_LOAD)
      continue;
    memsize += alloc_userspace(pgt, ph.p_vaddr, ino, ph.p_offset, ph.p_memsz);
  }

  kinfo("exec: userspace setup\n");

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

  strcpy(p->name, path);

  kinfo("exec complete!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

  return argc;  /* p->tf->x0 */

fail:
  printk("kernel exec failed\n");
  return -1;
}

void sleep(void *chan, struct spinlock *lk) {
  kinfo("sleeep\n");

  struct proc *p = myproc();

  if(lk != &proctable.lk) {
    acquire(&proctable.lk);
    release(lk);
  }

  p->chan = chan;
  p->state = SLEEPING;
  cswitch(&p->context, &mycpu()->scheduler);

  p->chan = NULL;

  if(lk != &proctable.lk)
    acquire(lk);
}

int wait(int *status) {
  kinfo("wait\n");
  struct proc *p = myproc();

  for(;;) {
    kinfo("waita\n");
    acquire(&proctable.lk);

    for(int i = 0; i < NPROC; i++) {
      struct proc *cp = &proctable.procs[i];
      if(cp->parent != p)
        continue;

      if(cp->state == ZOMBIE) {
        int pid = cp->pid;
        if(status)
          *status = cp->ret;

        freeproc(cp);

        release(&proctable.lk);

        return pid;
      }
    }

    // release(&proctable.lk);

    sleep(p, &proctable.lk);
    kinfo("from sleep\n");
  }
}

void wakeup(void *chan) {
  kinfo("wakeup\n");

  acquire(&proctable.lk);

  for(int i = 0; i < NPROC; i++) {
    struct proc *p = &proctable.procs[i];
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
  }

  release(&proctable.lk);
}

void exit(int ret) {
  kinfo("exit\n");
  struct proc *p = myproc();

  free_userspace(p->pgt, p->size);

  p->ret = ret;

  wakeup(p->parent);

  acquire(&proctable.lk);
  p->state = ZOMBIE;

  cswitch(&p->context, &mycpu()->scheduler);

  panic("unreachable");
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

void dump_proc(struct proc *p) {
  ;
}

void dump_kstack(struct proc *p) {
  for(int i = 0; i < PAGESIZE; i++) {
    printk("%x ", p->kstack[i]);
  }
}

void proc_init() {
  for(int i = 0; i < NCPU; i++) {
    memset(&cpus[i], 0, sizeof(cpus[i]));
  }

  lock_init(&pidalloc.lk);
  pidalloc.pid = 0;

  lock_init(&proctable.lk);
  memset(&proctable.procs, 0, sizeof(proctable.procs));
}
