#include "syscall.h"

.global initcode
initcode:
  ldr x0, =init
  ldr x1, =argv 
  mov x6, SYS_exec
  svc #0

init:
  .asciz "/init"
.p2align 3
argv:
  .quad init
  .quad 0
