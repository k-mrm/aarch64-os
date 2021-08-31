/* attr index */
#define AI_DEVICE_nGnRnE_IDX  0x0
#define AI_NORMAL_NC_IDX      0x1

#define AI_DEVICE_nGnRnE  0x0
#define AI_NORMAL_NC      0x44

#define VA_START    0xffffff8000000000

#define V2P(v)  ((v) - VA_START)
#define P2V(p)  ((p) + VA_START)

#define TCR_T0SZ(n)   (n & 0x3f)
#define TCR_IRGN0(n)  ((n & 0x3) << 8)
#define TCR_ORGN0(n)  ((n & 0x3) << 10)
#define TCR_SH0(n)    ((n & 0x3) << 12)
#define TCR_TG0(n)    ((n & 0x1) << 14)
#define TCR_T1SZ(n)   ((n & 0x3f) << 16)
#define TCR_A1(n)     ((n & 0x1) << 22)
#define TCR_EPD1(n)   ((n & 0x1) << 23)
#define TCR_IRGN1(n)  ((n & 0x3) << 24)
#define TCR_ORGN1(n)  ((n & 0x3) << 26)
#define TCR_SH1(n)    ((n & 0x3) << 28)
#define TCR_TG1(n)    ((n & 0x1) << 30)
#define TCR_IPS(n)    ((n & 0x7) << 32)
#define TCR_AS(n)     ((n & 0x1) << 36)
#define TCR_TBI0(n)   ((n & 0x1) << 37)
#define TCR_TBI1(n)   ((n & 0x1) << 38)

/* lower attribute */
#define PTE_VALID 1
#define PTE_TABLE 2
#define PTE_INDX(idx) (((idx) & 7) << 2)
#define PTE_NS  (1 << 5)
#define PTE_AP(ap)  (((ap) & 3) << 6)
#define PTE_SH(sh)  (((sh) & 3) << 8)
#define PTE_AF  (1 << 10)
/* upper attribute */
#define PTE_PXN (1 << 53)
#define PTE_UXN (1 << 54)

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
  ldr x0, =(V2P(vectortable))
  msr vbar_el1, x0

  /* setup mmu */
  /*
   *  map 0x0 - kend to 0xffffff8000000000 -
   *
   *
   */

  /*  setup l2pgt  */
  ldr x0, =V2P(l2kpgt0)
  ldr x1, =V2P(l2kpgt1)
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
  and x7, x7, #0x1ff /* start va level2 index */
  lsr x8, x6, #21
  and x8, x8, #0x1ff /* end va level2 index */
  lsr x9, x2, #21    /* start pa for block entry */
  mov x10, #(PTE_AF | PTE_INDX(AI_DEVICE_nGnRnE_IDX) | PTE_VALID) /* entry attr */
  orr x9, x9, x10 /* block entry */
l2pgt_loop0:
  str x9, [x0, x7, lsl #3]  /* l2kpgt0[l2idx] = block entry */
  add x7, x7, #1 /* next index */
  add x9, x9, #0x200000 /* next block, block size is 2MB */
  cmp x7, x8
  b.ls l2pgt_loop0  /* if start va idx <= end va idx */

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
  lsr x9, x2, #21    /* start pa for block entry */
  mov x10, #(PTE_AF | PTE_INDX(AI_NORMAL_NC_IDX) | PTE_VALID) /* entry attr */
  orr x9, x9, x10 /* block entry */
l2pgt_loop1:
  str x9, [x1, x7, lsl #3]  /* l2kpgt1[l2idx] = block entry */
  add x7, x7, #1 /* next index */
  add x9, x9, #0x200000 /* next block, block size is 2MB */
  cmp x7, x8
  b.ls l2pgt_loop1  /* if start va idx <= end va idx */

  /* setup l1pgt */
  ldr x0, =V2P(l1kpgt)

  mov x1, #0x8000000    /* start pa */
  ldr x2, =V2P(kend)-1  /* end pa */
  mov x4, #VA_START
  add x3, x1, x4    /* start va */
  add x4, x2, x4    /* end va */

  lsr x5, x3, #30
  and x5, x5, #0x1ff  /* start va level1 index */ 

  mov x6, #(PTE_TABLE | PTE_VALID)  /* entry attr */
  ldr x7, =V2P(l2kpgt0)
  ldr x8, =V2P(l2kpgt1)
  orr x7, x7, x6    /* table entry */
  orr x8, x8, x6    /* table entry */

  str x7, [x0, x5, lsl #3]    /* l1kpgt[l1idx] = table entry */
  add x5, x5, #1    /* next index */
  str x8, [x0, x5, lsl #3]    /* l1kpgt[l1idx] = table entry */

  /* setup ttbr1_el1 */
  msr ttbr1_el1, x0

  /* setup stack */
  ldr x1, =__kstack
  mov sp, x1

  /* setup tcr */
  ldr x0, =(TCR_T0SZ(25) | TCR_T1SZ(25) | TCR_TG0(0) | TCR_TG1(2) | TCR_IPS(0))
  msr tcr_el1, x0

  /* setup mair */
  ldr x1, =((AI_DEVICE_nGnRnE << (8 * AI_DEVICE_nGnRnE_IDX)) | (AI_NORMAL_NC << (8 * AI_NORMAL_NC_IDX)))
  msr mair_el1, x1

  isb

  ldr x1, =hang

  /* enable paging */
  mrs x0, sctlr_el1
  orr x0, x0, #1
  msr sctlr_el1, x0

  br x1

  /* clear bss */
  ldr x1, =__bss_start
  ldr w2, =__bss_size
1:
  cbz w2, 2f
  str xzr, [x1], #8
  sub w2, w2, #1
  b 1b
2:


.global hang
hang:
  wfe
  b hang

.section ".data"

.align 12
l1kpgt:
  .zero 4096 
.align 12
l2kpgt0:
  .zero 4096
.align 12
l2kpgt1:
  .zero 4096
