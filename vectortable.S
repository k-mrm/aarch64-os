.global vectortable
.balign 0x800
vectortable:
.balign 0x80
  b hang
.balign 0x80
  b hang
.balign 0x80
  b hang
.balign 0x80
  b hang

.balign 0x80
  b cur_sp_elx_sync
.balign 0x80
  b cur_sp_elx_irq
.balign 0x80
  b hang
.balign 0x80
  b hang

.balign 0x80
  b hang
.balign 0x80
  b hang
.balign 0x80
  b hang
.balign 0x80
  b hang

.balign 0x80
  b hang
.balign 0x80
  b hang
.balign 0x80
  b hang
.balign 0x80
  b hang

cur_sp_elx_sync:
  /* save register */
  stp x0, x1, [sp, #-16]!
  stp x2, x3, [sp, #-16]!
  stp x4, x5, [sp, #-16]!
  stp x6, x7, [sp, #-16]!
  stp x8, x9, [sp, #-16]!
  stp x10, x11, [sp, #-16]!
  stp x12, x13, [sp, #-16]!
  stp x14, x15, [sp, #-16]!
  stp x16, x17, [sp, #-16]!
  stp x18, x19, [sp, #-16]!
  stp x20, x21, [sp, #-16]!
  stp x22, x23, [sp, #-16]!
  stp x24, x25, [sp, #-16]!
  stp x26, x27, [sp, #-16]!
  stp x28, x29, [sp, #-16]!

  mrs x1, elr_el1
  mrs x2, spsr_el1

  stp lr, x1, [sp, #-16]!
  str x2, [sp, #-8]!

  bl  sync_handler

  /* restore register */
  ldr x2, [sp], 8
  ldp lr, x1, [sp], 16

  msr elr_el1, x1
  msr spsr_el1, x2

  ldp x28, x29, [sp], 16
  ldp x26, x27, [sp], 16
  ldp x24, x25, [sp], 16
  ldp x22, x23, [sp], 16
  ldp x20, x21, [sp], 16
  ldp x18, x19, [sp], 16
  ldp x16, x17, [sp], 16
  ldp x14, x15, [sp], 16
  ldp x12, x13, [sp], 16
  ldp x10, x11, [sp], 16
  ldp x8, x9, [sp], 16
  ldp x6, x7, [sp], 16
  ldp x4, x5, [sp], 16
  ldp x2, x3, [sp], 16
  ldp x0, x1, [sp], 16

  eret
cur_sp_elx_irq:
