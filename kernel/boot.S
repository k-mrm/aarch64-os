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


.section ".text.boot"

.global _start

_start:
  mrs x1, mpidr_el1
  and x1, x1, #3
  cbz x1, kernel_start

  b hang

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
   *  map 0x0 ~ kernel end to 0xffffff8000000000 ~ 
   *
   *
   */
  ldr x0, =pgtable
  msr ttbr0_el1, x0

  /* setup tcr */
  ldr x1, =(TCR_T1SZ(25) | TCR_TG1(2) | TCR_IPS(0))
  msr tcr_el1, x1

  /* setup mair */
  ldr x2, =((AI_DEVICE_nGnRnE << (8 * AI_DEVICE_nGnRnE_IDX)) | (AI_NORMAL_NC << (8 * AI_NORMAL_NC_IDX)))
  msr mair_el1, x2

  isb

  /* enable paging */
  mrs x0, sctlr_el1
  orr x0, x0, #1
  msr sctlr_el1, x0

  adrp x1, main_kstack
  add x1, x1, #4096
  mov sp, x1

  /* clear bss */
  ldr x1, =__bss_start
  ldr w2, =__bss_size
1:
  cbz w2, 2f
  str xzr, [x1], #8
  sub w2, w2, #1
  b 1b
2:

  bl main

.global hang
hang:
  wfe
  b hang

.section ".data"

.align 12
pgtable:
  .zero 4096 

