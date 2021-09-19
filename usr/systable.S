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
SYSCALL exit, SYS_EXIT
SYSCALL fork, SYS_FORK
