.section ".text"

.macro ksavereg
  sub sp, sp, #272
  stp x0, x1, [sp, #16 * 0]
  stp x2, x3, [sp, #16 * 1]
  stp x4, x5, [sp, #16 * 2]
  stp x6, x7, [sp, #16 * 3]
  stp x8, x9, [sp, #16 * 4]
  stp x10, x11, [sp, #16 * 5]
  stp x12, x13, [sp, #16 * 6]
  stp x14, x15, [sp, #16 * 7]
  stp x16, x17, [sp, #16 * 8]
  stp x18, x19, [sp, #16 * 9]
  stp x20, x21, [sp, #16 * 10]
  stp x22, x23, [sp, #16 * 11]
  stp x24, x25, [sp, #16 * 12]
  stp x26, x27, [sp, #16 * 13]
  stp x28, x29, [sp, #16 * 14]

  mrs x9, elr_el1
  mrs x10, spsr_el1
  add x11, sp, #272

  stp x30, x9, [sp, #16 * 15]
  stp x10, x11, [sp, #16 * 16]
.endm

.macro savereg
  sub sp, sp, #272
  stp x0, x1, [sp, #16 * 0]
  stp x2, x3, [sp, #16 * 1]
  stp x4, x5, [sp, #16 * 2]
  stp x6, x7, [sp, #16 * 3]
  stp x8, x9, [sp, #16 * 4]
  stp x10, x11, [sp, #16 * 5]
  stp x12, x13, [sp, #16 * 6]
  stp x14, x15, [sp, #16 * 7]
  stp x16, x17, [sp, #16 * 8]
  stp x18, x19, [sp, #16 * 9]
  stp x20, x21, [sp, #16 * 10]
  stp x22, x23, [sp, #16 * 11]
  stp x24, x25, [sp, #16 * 12]
  stp x26, x27, [sp, #16 * 13]
  stp x28, x29, [sp, #16 * 14]

  mrs x9, elr_el1
  mrs x10, spsr_el1
  mrs x11, sp_el0

  stp x30, x9, [sp, #16 * 15]
  stp x10, x11, [sp, #16 * 16]
.endm

.macro restorereg
  ldp x30, x9, [sp, #16 * 15]
  ldp x10, x11, [sp, #16 * 16]

  msr elr_el1, x9
  msr spsr_el1, x10
  msr sp_el0, x11

  ldp x0, x1, [sp, #16 * 0]
  ldp x2, x3, [sp, #16 * 1]
  ldp x4, x5, [sp, #16 * 2]
  ldp x6, x7, [sp, #16 * 3]
  ldp x8, x9, [sp, #16 * 4]
  ldp x10, x11, [sp, #16 * 5]
  ldp x12, x13, [sp, #16 * 6]
  ldp x14, x15, [sp, #16 * 7]
  ldp x16, x17, [sp, #16 * 8]
  ldp x18, x19, [sp, #16 * 9]
  ldp x20, x21, [sp, #16 * 10]
  ldp x22, x23, [sp, #16 * 11]
  ldp x24, x25, [sp, #16 * 12]
  ldp x26, x27, [sp, #16 * 13]
  ldp x28, x29, [sp, #16 * 14]
  add sp, sp, #272
.endm

.global vectortable
.balign 0x800
vectortable:
/* Current EL with sp0 */
  mov x0, 0
  b unknownint
.balign 0x80
  mov x0, 1
  b unknownint
.balign 0x80
  mov x0, 2
  b unknownint
.balign 0x80
  mov x0, 3
  b unknownint

/* Current EL with spx */
.balign 0x80
  b el1h_sync
.balign 0x80
  b el1h_irq
.balign 0x80
  mov x0, 4
  b unknownint
.balign 0x80
  mov x0, 5
  b unknownint

/* Lower EL using aarch64 */
.balign 0x80
  b el0_sync
.balign 0x80
  b el0_irq
.balign 0x80
  mov x0, 8
  b unknownint
.balign 0x80
  mov x0, 9
  b unknownint

/* Lower EL using aarch32 */
.balign 0x80
  mov x0, 10
  b unknownint
.balign 0x80
  mov x0, 11
  b unknownint
.balign 0x80
  mov x0, 12
  b unknownint
.balign 0x80
  mov x0, 13
  b unknownint

debugtrap:
  savereg

  mov x0, sp
  bl  debug_trap

  restorereg

  eret

el1h_sync:
  ksavereg

  mov x0, sp
  bl  ksync_handler

  restorereg

  eret

el1h_irq:
  ksavereg

  mov x0, sp
  bl  kirq_handler

  restorereg

  eret

el0_sync:
  savereg

  mov x0, sp
  bl  usync_handler

  restorereg

  eret

el0_irq:
  savereg

  mov x0, sp
  bl  uirq_handler

.global trapret
trapret:
  restorereg

  eret
