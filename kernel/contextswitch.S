.global swtch

swtch: /* swtch(struct context *old, struct context *new) */
  mov x9, sp

  stp x9, x0, [x0, #16 * 0]
  stp x1, x2, [x0, #16 * 1]
  stp x3, x4, [x0, #16 * 2]
  stp x5, x6, [x0, #16 * 3]
  stp x7, x8, [x0, #16 * 4]
  stp x16, x17, [x0, #16 * 5]
  stp x18, x19, [x0, #16 * 6]
  stp x20, x21, [x0, #16 * 7]
  stp x22, x23, [x0, #16 * 8]
  stp x24, x25, [x0, #16 * 9]
  stp x26, x27, [x0, #16 * 10]
  stp x28, x29, [x0, #16 * 11]
  stp x30, xzr, [x0, #16 * 12]

  ldp x9, x0, [x1, #16 * 0]
  ldp x1, x2, [x1, #16 * 1]
  ldp x3, x4, [x1, #16 * 2]
  ldp x5, x6, [x1, #16 * 3]
  ldp x7, x8, [x1, #16 * 4]
  ldp x16, x17, [x1, #16 * 5]
  ldp x18, x19, [x1, #16 * 6]
  ldp x20, x21, [x1, #16 * 7]
  ldp x22, x23, [x1, #16 * 8]
  ldp x24, x25, [x1, #16 * 9]
  ldp x26, x27, [x1, #16 * 10]
  ldp x28, x29, [x1, #16 * 11]
  ldp x30, xzr, [x1, #16 * 12]
  
  ret // lr(x30) == forkret
