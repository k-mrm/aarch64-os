#include "syscall.h"

.global getpid
getpid:
  mov x6, #SYS_GETPID
  svc #0
  ret
