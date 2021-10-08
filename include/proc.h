#ifndef AARCH64_OS_PROC_H
#define AARCH64_OS_PROC_H

#include "kernel.h"
#include "ext2.h"
#include "file.h"

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

enum procstate {
  UNUSED,
  CREATED,
  RUNNING,
  RUNNABLE,
  SLEEPING,
  ZOMBIE,
};

struct proc {
  enum procstate state;
  int ret;
  pid_t pid;
  u64 size;
  struct context context;
  struct trapframe *tf;
  struct proc *parent;
  char *kstack;
  u64 *pgt;
  struct inode *cwd;
  struct file *ofile[16];
  char name[16];
};

#define NPROC 256

void proc_init(void);
void schedule(void);
struct proc *newproc(void);

void cswitch(struct context *old, struct context *new);

void forkret(void);

void yield(void);

extern struct proc proctable[NPROC];
extern struct proc *curproc;

#endif
