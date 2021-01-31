.section ".text.boot"

.global _start

_start:
  ldr x1, =_start
  mov sp, x1

  bl main

loop:
  wfe
  b loop
