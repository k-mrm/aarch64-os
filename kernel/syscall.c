#include "trap.h"
#include "proc.h"
#include "syscall.h"
#include "printk.h"
#include "log.h"
#include "file.h"
#include "driver/timer.h"

typedef int (*syscall_t)(struct trapframe *tf);

struct stat;
struct utsname;

int getpid(void);
int write(int fd, char *s, u64 size);
int read(int fd, char *buf, u64 size);
int exit(int ret);
int fork(void);
int clone(void *fn, void *stack);
int wait(int *status);
int exec(char *path, char **argv);
int open(char *path, int flags);
int close(int fd);
int fstat(int fd, struct stat *st);
int uname(struct utsname *u);
int chdir(char *path);
int mkdir(char *path);
int mknod(char *path, int mode, int dev);
int dup(int fd);
int clone(void *fn, void *stack);
int waitpid(int pid, int *status);

u64 sysarg(struct trapframe *tf, int n) {
  switch(n) {
    case 0:
      return tf->x0;
    case 1:
      return tf->x1;
    case 2:
      return tf->x2;
    case 3:
      return tf->x3;
  }

  panic("invalid sysarg");
}

int sys_getpid(struct trapframe *tf) {
  return getpid();
}

int sys_write(struct trapframe *tf) {
  kinfo("writesyscall! %p\n", tf->elr);
  int fd = sysarg(tf, 0);
  u64 str = sysarg(tf, 1);
  u64 size = sysarg(tf, 2);
  return write(fd, (char *)str, size);
}

int sys_read(struct trapframe *tf) {
  int fd = sysarg(tf, 0);
  u64 buf = sysarg(tf, 1);
  u64 size = sysarg(tf, 2);
  return read(fd, (char *)buf, size);
}

int sys_exit(struct trapframe *tf) {
  int ret = sysarg(tf, 0);
  exit(ret);
  return 0; /* unreachable */
}

int sys_fork(struct trapframe *tf) {
  return fork();
}

int sys_clone(struct trapframe *tf) {
  u64 fn = sysarg(tf, 0);
  u64 stack = sysarg(tf, 1);
  return clone((void *)fn, (void *)stack);
}

int sys_wait(struct trapframe *tf) {
  u64 addr = sysarg(tf, 0);
  return wait((int *)addr);
}

int sys_exec(struct trapframe *tf) {
  u64 path = sysarg(tf, 0);
  u64 argv = sysarg(tf, 1);
  return exec((char *)path, (char **)argv);
}

int sys_open(struct trapframe *tf) {
  u64 path = sysarg(tf, 0);
  int flags = sysarg(tf, 1);
  return open((char *)path, flags);
}

int sys_close(struct trapframe *tf) {
  int fd = sysarg(tf, 0);
  return close(fd);
}

int sys_fstat(struct trapframe *tf) {
  int fd = sysarg(tf, 0);
  struct stat *addr = (struct stat *)sysarg(tf, 1);
  return fstat(fd, addr);
}

int sys_uname(struct trapframe *tf) {
  struct utsname *addr = (struct utsname *)sysarg(tf, 0);
  return uname(addr);
}

int sys_chdir(struct trapframe *tf) {
  u64 path = sysarg(tf, 0);
  return chdir((char *)path);
}

int sys_mkdir(struct trapframe *tf) {
  u64 path = sysarg(tf, 0);
  return mkdir((char *)path);
}

int sys_mknod(struct trapframe *tf) {
  u64 path = sysarg(tf, 0);
  int mode = sysarg(tf, 1);
  int dev = sysarg(tf, 2);
  return mknod((char *)path, mode, dev);
}

int sys_dup(struct trapframe *tf) {
  int fd = sysarg(tf, 0);
  return dup(fd);
}

int sys_waitpid(struct trapframe *tf) {
  int pid = sysarg(tf, 0);
  u64 status = sysarg(tf, 1);
  return waitpid(pid, (int *)status);
}

int sys_ticks(struct trapframe *tf) {
  (void)tf;
  return ticks;
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
  sys_mkdir,
  sys_mknod,
  sys_dup,
  sys_clone,
  sys_waitpid,
  sys_ticks,
};

void syscall(struct trapframe *tf) {
  int n = tf->x6;

  if(n < NSYSCALL && syscall_table[n]) {
    tf->x0 = syscall_table[n](tf);
  } else {
    panic("unknown syscall");
    tf->x0 = -1;
  }
}
