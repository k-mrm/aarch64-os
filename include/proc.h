#ifndef AARCH64_OS_PROC_H
#define AARCH64_OS_PROC_H

#include "kernel.h"
#include "fs.h"
#include "file.h"
#include "spinlock.h"

typedef int pid_t;

struct context {
  u64 sp;
  /* callee register */
  u64 x0;
  u64 x1;
  u64 x2;
  u64 x3;
  u64 x4;
  u64 x5;
  u64 x6;
  u64 x7;
  u64 x8;
  u64 x16;
  u64 x17;
  u64 x18;
  u64 x19;
  u64 x20;
  u64 x21;
  u64 x22;
  u64 x23;
  u64 x24;
  u64 x25;
  u64 x26;
  u64 x27;
  u64 x28;
  u64 x29;
  u64 lr;  /* x30 */
} __attribute__((aligned(16)));

#define NCPU 2

struct cpu {
  struct proc *proc;
  struct context scheduler;
  int cli_depth;
};

struct cpu cpus[NCPU];

enum procstate {
  UNUSED,
  CREATED,
  RUNNING,
  RUNNABLE,
  SLEEPING,
  ZOMBIE,
};

#define NOFILE  16

struct proc {
  enum procstate state;
  int ret;
  pid_t pid;
  u64 size;
  void *chan;
  struct context context;
  struct trapframe *tf;
  struct proc *parent;
  char *kstack;
  u64 *pgt;
  struct inode *cwd;
  struct file *ofile[NOFILE];
  char name[16];
  int has_th;
  int th;
  int (*fault_handler)(struct proc *);
};

#define NPROC 256

struct cpu *mycpu(void);
struct proc *myproc(void);

void proc_init(void);
void userproc_init(void);

void schedule(void);

void sleep(void *chan, struct spinlock *lk);
void wakeup(void *chan);
void yield(void);

void dumpps(void);

int copyout(struct proc *p, void *udst, const void *src, u64 sz);
int copyin(struct proc *p, void *dst, const void *usrc, u64 sz);

#endif
