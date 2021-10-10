#include "syscall.h" 
.macro SYSCALL name, id
.global \name
\name:
  mov x6, #\id
  svc #0
  ret
.endm

SYSCALL getpid, SYS_GETPID
SYSCALL write, SYS_WRITE
SYSCALL read, SYS_READ
SYSCALL exit, SYS_EXIT
SYSCALL fork, SYS_FORK
SYSCALL wait, SYS_WAIT
SYSCALL exec, SYS_EXEC
SYSCALL open, SYS_OPEN
SYSCALL close, SYS_CLOSE
SYSCALL fstat, SYS_FSTAT
