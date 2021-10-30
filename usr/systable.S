#include "syscall.h" 
.macro SYSCALL name, id
.global \name
\name:
  mov x6, #\id
  svc #0
  ret
.endm

SYSCALL getpid, SYS_getpid
SYSCALL write, SYS_write
SYSCALL read, SYS_read
SYSCALL exit, SYS_exit
SYSCALL fork, SYS_fork
SYSCALL wait, SYS_wait
SYSCALL exec, SYS_exec
SYSCALL open, SYS_open
SYSCALL close, SYS_close
SYSCALL fstat, SYS_fstat
SYSCALL uname, SYS_uname
SYSCALL chdir, SYS_chdir
SYSCALL mkdir, SYS_mkdir
SYSCALL mknod, SYS_mknod
