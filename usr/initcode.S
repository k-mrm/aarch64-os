#include "syscall.h"

.global initcode
initcode:
  ldr x0, =init
  ldr x1, =argv 
  mov x6, SYS_EXEC
  svc #0

init:
  .asciz "/init"
.align 3
argv:
  .quad init
  .quad 0
