#include "trap.h"
#include "proc.h"
#include "syscall.h"
#include "printk.h"

typedef int (*syscall_t)(void);

u64 sysarg(int n) {
  switch(n) {
    case 0:
      return curproc->tf->x0;
    case 1:
      return curproc->tf->x1;
    case 2:
      return curproc->tf->x2;
    case 3:
      return curproc->tf->x3;
  }

  return 0;
}

int sys_getpid(void) {
  return getpid();
}

int sys_write(void) {
  u64 str = sysarg(0);
  u64 size = sysarg(1);
  return write((char *)str, size);
}

int sys_read(void) {
  u64 buf = sysarg(0);
  u64 size = sysarg(1);
  return read((char *)buf, size);
}

int sys_exit(void) {
  int ret = sysarg(0);
  exit(ret);
  return 0;
}

int sys_fork(void) {
  return fork();
}

int sys_wait(void) {
  u64 addr = sysarg(0);
  return wait((int *)addr);
}

syscall_t syscall_table[] = {
  sys_getpid,
  sys_write,
  sys_read,
  sys_exit,
  sys_fork,
  sys_wait,
};

void syscall(struct trapframe *tf) {
  int n = tf->x6;

  if(n < NSYSCALL && syscall_table[n]) {
    tf->x0 = syscall_table[n]();
  } else {
    panic("unknown syscall");
    tf->x0 = -1;
  }
}
