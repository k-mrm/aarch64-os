.section ".text.boot"

.global _start

_start:
  mrs x1, mpidr_el1
  and x1, x1, 3
  cbz x1, kernel_start

  b loop

kernel_start:
  ldr x1, =_start
  mov sp, x1

  bl main

loop:
  wfe
  b loop
