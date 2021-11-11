#include "mm.h"
#include "memmap.h"

.section ".text.boot"

.global _start

_start:
  mrs x1, mpidr_el1
  and x1, x1, #3
  cbz x1, kernel_start

/* switch EL2 to EL1 */
/*
swtch_el1:
  mov x0, #(1 << 31) 
  orr x0, x0, #(1 << 1)
  msr hcr_el2, x0

  adr x0, kernel_start
  msr elr_el2, x0

  mov x0, #0x3c5  // DAIF=1, M=0b101(EL1H)
  msr spsr_el2, x0

  eret
  */

kernel_start:
  /* setup mmu */
  /* map 0x40000000 - PA(kend) to 0x40000000 - PA(kend) (identity mapping) */
  /* setup l2upgt */
  adrp x0, l2upgt

  mov x1, #0x40000000   /* start pa/va */
  ldr x2, =V2P(kend)-1  /* end pa/va */

  lsr x3, x1, #21
  and x3, x3, #0x1ff /* start va level2 index */
  lsr x4, x2, #21
  and x4, x4, #0x1ff /* end va level2 index */
  mov x5, #(PTE_AF | PTE_INDX(AI_NORMAL_NC_IDX) | PTE_VALID) /* entry attr */
  orr x6, x1, x5 /* block entry */
l2upgt_loop:
  str x6, [x0, x3, lsl #3]  /* l2upgt1[l2idx] = block entry */
  add x3, x3, #1    /* next index */
  add x6, x6, #0x200000   /* next block, block size is 2MB */
  cmp x3, x4
  b.ls l2upgt_loop    /* if start va idx <= end va idx */

  /* setup l1upgt */
  adrp x0, l1upgt

  mov x1, #0x40000000   /* start pa/va */
  ldr x2, =V2P(kend)-1  /* end pa/va */

  lsr x3, x1, #30
  and x3, x3, #0x1ff  /* start va level1 index */ 

  mov x4, #(PTE_TABLE | PTE_VALID)  /* entry attr */
  adrp x5, l2upgt
  orr x6, x4, x5    /* table entry */

  str x6, [x0, x3, lsl #3]    /* l1upgt[l1idx] = table entry */

  /* map 0xffffff8008000000 - VA(kend) to 0x8000000 - PA(kend)*/

  /*  setup l2pgt  */
  adrp x0, l2kpgt0
  adrp x1, l2kpgt1

  /*
   *  0x8000000 - 0x3fffffff
   *
   *  Peripheral
   *  Device memory
   */
  mov x2, #0x8000000  /* start pa */
  mov x3, #0x3fffffff /* end pa */
  mov x4, #VA_START
  add x5, x2, x4      /* start va */
  add x6, x3, x4      /* end va */

  lsr x7, x5, #21
  and x7, x7, #0x1ff  /* start va level2 index */
  lsr x8, x6, #21
  and x8, x8, #0x1ff  /* end va level2 index */
  mov x10, #(PTE_AF | PTE_INDX(AI_DEVICE_nGnRnE_IDX) | PTE_VALID) /* entry attr */
  orr x9, x2, x10   /* block entry */
l2pgt_loop0:
  str x9, [x0, x7, lsl #3]    /* l2kpgt0[l2idx] = block entry */
  add x7, x7, #1  /* next index */
  add x9, x9, #0x200000   /* next block, block size is 2MB */
  cmp x7, x8
  b.ls l2pgt_loop0    /* if start va idx <= end va idx */

  /*
   *  0x40000000 - (PA(kend)-1)
   *
   *  kernel section
   *  normal memory
   */
  mov x2, #0x40000000  /* start pa */
  ldr x3, =V2P(kend)-1  /* end pa */
  mov x4, #VA_START
  add x5, x2, x4      /* start va */
  add x6, x3, x4      /* end va */

  lsr x7, x5, #21
  and x7, x7, #0x1ff /* start va level2 index */
  lsr x8, x6, #21
  and x8, x8, #0x1ff /* end va level2 index */
  mov x10, #(PTE_AF | PTE_INDX(AI_NORMAL_NC_IDX) | PTE_VALID) /* entry attr */
  orr x9, x2, x10 /* block entry */
l2pgt_loop1:
  str x9, [x1, x7, lsl #3]  /* l2kpgt1[l2idx] = block entry */
  add x7, x7, #1 /* next index */
  add x9, x9, #0x200000 /* next block, block size is 2MB */
  cmp x7, x8
  b.ls l2pgt_loop1  /* if start va idx <= end va idx */

  /* setup l1pgt */
  adrp x0, l1kpgt

  mov x1, #0x8000000    /* start pa */
  ldr x2, =V2P(kend)-1  /* end pa */
  mov x4, #VA_START
  add x3, x1, x4    /* start va */
  add x4, x2, x4    /* end va */

  lsr x5, x3, #30
  and x5, x5, #0x1ff  /* start va level1 index */ 

  mov x6, #(PTE_TABLE | PTE_VALID)  /* entry attr */
  adrp x7, l2kpgt0
  adrp x8, l2kpgt1
  orr x7, x7, x6    /* table entry */
  orr x8, x8, x6    /* table entry */

  str x7, [x0, x5, lsl #3]    /* l1kpgt[l1idx] = table entry */
  add x5, x5, #1    /* next index */
  str x8, [x0, x5, lsl #3]    /* l1kpgt[l1idx] = table entry */

  /* setup ttbr0/1_el1 */
  adrp x0, l1upgt
  msr ttbr0_el1, x0
  adrp x0, l1kpgt
  msr ttbr1_el1, x0

  /* setup tcr */
  ldr x0, =(TCR_T0SZ(25) | TCR_T1SZ(25) | TCR_TG0(0) | TCR_TG1(2) | TCR_IPS(0))
  msr tcr_el1, x0

  /* setup mair */
  ldr x1, =((AI_DEVICE_nGnRnE << (8 * AI_DEVICE_nGnRnE_IDX)) | (AI_NORMAL_NC << (8 * AI_NORMAL_NC_IDX)))
  msr mair_el1, x1

  isb

  /* setup stack and kernel main */
  ldr x0, =vectortable
  msr vbar_el1, x0

  ldr x1, =__kstack
  mov sp, x1

  ldr x2, =clearbss

  /* enable paging */
  mrs x0, sctlr_el1
  orr x0, x0, #1
  msr sctlr_el1, x0

  isb

  br x2

clearbss:
  ldr x1, =__bss_start
  ldr w2, =__bss_size
1:
  cbz w2, 2f
  str xzr, [x1], #8
  sub w2, w2, #1
  b 1b
2:

  b main

.global hang
hang:
  wfe
  b hang

.section ".data"

.align 12
.global l1kpgt
l1kpgt:
  .zero 4096 
.align 12
l2kpgt0:
  .zero 4096
.align 12
l2kpgt1:
  .zero 4096

.align 12
l1upgt:
  .zero 4096
.align 12
l2upgt:
  .zero 4096
