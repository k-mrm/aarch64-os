#include "trap.h"
#include "syscall.h"

typedef int (*syscall_t)(void);

int sys_getpid(void);

syscall_t syscall_table[NSYSCALL] = {
  sys_getpid,
};

void syscall(struct trapframe *tf) {
  int n = tf->x6;
  int ret = syscall_table[n]();
  tf->x0 = ret;
}
