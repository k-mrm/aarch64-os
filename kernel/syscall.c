#include "trap.h"
#include "proc.h"
#include "syscall.h"

typedef int (*syscall_t)(void);

u64 sysarg(int n) {
  switch(n) {
    case 0:
      return curproc->tf->x0;
    case 1:
      return curproc->tf->x1;
    case 2:
      return curproc->tf->x2;
  }

  return 0;
}

int sys_getpid(void) {
  return _getpid();
}

int sys_write(void) {
  u64 str = sysarg(0);
  u64 size = sysarg(1);
  return _write((char *)str, size);
}

int sys_exit(void) {
  int ret = sysarg(0);
  return _exit(ret);
}

syscall_t syscall_table[] = {
  sys_getpid,
  sys_write,
  sys_exit,
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
