/* attr index */
#define AI_DEVICE_nGnRnE_IDX  0x0
#define AI_NORMAL_NC_IDX      0x1

#define AI_DEVICE_nGnRnE  0x0
#define AI_NORMAL_NC      0x44

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

.align 12
pgtable:
  .zero 4096 

kernel_start:
  ldr x0, =vectortable
  msr vbar_el1, x0

  /* setup mmu */
  ldr x0, =pgtable
  msr ttbr0_el1, x0

  ldr x0, =((AI_DEVICE_nGnRnE << (8 * AI_DEVICE_nGnRnE_IDX)) | (AI_NORMAL_NC << (8 * AI_NORMAL_NC_IDX)))
  msr mair_el1, x0

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
