#ifndef MONO_PROC_H
#define MONO_PROC_H

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
  u64 x30;  /* lr */
};

enum procstate {
  CREATED,
  RUNNING,
  RUNNABLE,
  SLEEP,
  DEAD,
};

struct proc {
  enum procstate state;

  pid_t pid;

  struct context context;
};

struct proc procs[128];

#endif
