#include "syscall.h"

.global initcode
initcode:
  ldr x0, =init
  mov x6, SYS_EXEC
  svc #0

init:
  .string "/init\0"
