#define SPSR_M_EL1H 0x5

.section ".text.boot"

.global _start

_start:
  mrs x1, mpidr_el1
  and x1, x1, #3
  cbz x1, swtch_el1

  b loop

/* switch EL2 to EL1 */
swtch_el1:
  mov x0, #(1 << 31) 
  orr x0, x0, #(1 << 1)
  msr hcr_el2, x0
  adr x0, kernel_start
  msr elr_el2, x0
  mov x0, SPSR_M_EL1H
  msr spsr_el2, x0
  eret

kernel_start:
  ldr x1, =_start
  mov sp, x1

  bl main

loop:
  wfe
  b loop
