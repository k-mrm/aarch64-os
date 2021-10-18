#include "trap.h"
#include "proc.h"
#include "syscall.h"
#include "printk.h"

typedef int (*syscall_t)(void);

struct stat;
struct utsname;

int getpid(void);
int write(int fd, char *s, u64 size);
int read(int fd, char *buf, u64 size);
int exit(int ret);
int fork(void);
int wait(int *status);
int exec(char *path, char **argv);
int open(char *path, int flags);
int close(int fd);
int fstat(int fd, struct stat *st);
int uname(struct utsname *u);

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
  int fd = sysarg(0);
  u64 str = sysarg(1);
  u64 size = sysarg(2);
  return write(fd, (char *)str, size);
}

int sys_read(void) {
  int fd = sysarg(0);
  u64 buf = sysarg(1);
  u64 size = sysarg(2);
  return read(fd, (char *)buf, size);
}

int sys_exit(void) {
  int ret = sysarg(0);
  exit(ret);
  return 0; /* unreachable */
}

int sys_fork(void) {
  return fork();
}

int sys_wait(void) {
  u64 addr = sysarg(0);
  return wait((int *)addr);
}

int sys_exec(void) {
  u64 path = sysarg(0);
  u64 argv = sysarg(1);
  return exec((char *)path, (char **)argv);
}

int sys_open(void) {
  u64 path = sysarg(0);
  int flags = sysarg(1);
  return open((char *)path, flags);
}

int sys_close(void) {
  int fd = sysarg(0);
  return close(fd);
}

int sys_fstat(void) {
  int fd = sysarg(0);
  struct stat *addr = (struct stat *)sysarg(1);
  return fstat(fd, addr);
}

int sys_uname(void) {
  struct utsname *addr = (struct utsname *)sysarg(0);
  return uname(addr);
}

int sys_chdir(void) {
  u64 path = sysarg(0);
  return chdir((char *)path);
}

syscall_t syscall_table[] = {
  sys_getpid,
  sys_write,
  sys_read,
  sys_exit,
  sys_fork,
  sys_wait,
  sys_exec,
  sys_open,
  sys_close,
  sys_fstat,
  sys_uname,
  sys_chdir,
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
