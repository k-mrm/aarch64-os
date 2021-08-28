#include "mono.h"
#include "aarch64.h"
#include "mm.h"
#include "kalloc.h"

#define TCR_T0SZ(n)   ((n) & 0x3f)
#define TCR_IRGN0(n)  (((n) & 0x3) << 8)
#define TCR_ORGN0(n)  (((n) & 0x3) << 10)
#define TCR_SH0(n)    (((n) & 0x3) << 12)
#define TCR_TG0(n)    (((n) & 0x1) << 14)
#define TCR_T1SZ(n)   (((n) & 0x3f) << 16)
#define TCR_A1(n)     (((n) & 0x1) << 22)
#define TCR_EPD1(n)   (((n) & 0x1) << 23)
#define TCR_IRGN1(n)  (((n) & 0x3) << 24)
#define TCR_ORGN1(n)  (((n) & 0x3) << 26)
#define TCR_SH1(n)    (((n) & 0x3) << 28)
#define TCR_TG1(n)    (((n) & 0x1) << 30)
#define TCR_IPS(n)    (((n) & 0x7) << 32)
#define TCR_AS(n)     (((n) & 0x1) << 36)
#define TCR_TBI0(n)   (((n) & 0x1) << 37)
#define TCR_TBI1(n)   (((n) & 0x1) << 38)

static __attribute__((aligned(4096))) u64 l1_pgt[512];
static __attribute__((aligned(4096))) u64 l2_pgt[512];
static __attribute__((aligned(4096))) u64 l3_pgt[512];

/*
 *  39bit(=512GB) Virtual Address
 *
 *     63   39       30       21       12           0
 *    +-------+--------+--------+--------+----------+
 *    | TTBRn | level1 | level2 | level3 | page off |
 *    +-------+--------+--------+--------+----------+
 *
 *
 */

#define PIDX(level, va) (((va) >> (39 - (level) * 9)) & 0x1ff)
#define OFFSET(va)  ((va) & 0xfff)

#define PTE_PA(pte) ((u64)(pte) & 0xfffffffff000)

/* lower attribute */
#define PTE_INDX(idx) (((idx) & 7) << 2)
#define PTE_NS  (1 << 5)
#define PTE_AP(ap)  (((ap) & 3) << 6)
#define PTE_SH(sh)  (((sh) & 3) << 8)
#define PTE_AF  (1 << 10)
/* upper attribute */
#define PTE_PXN (1 << 53)
#define PTE_UXN (1 << 54)

extern char *ktext_end;

void set_kpgt(u64 *k) {
  set_ttbr1_el1((u64)k);
}

void set_upgt(u64 *u) {
  set_ttbr0_el1((u64)u);
}

void kpgt_init() {
  u64 *kpgt = kalloc();

  set_kpgt(kpgt);
}

void upgt_init() {
  u64 *upgt = kalloc();

  set_upgt(upgt);
}

u64 *pagewalk(u64 *pgt, u64 va) {
  for(int level = 1; level < 3; level++) {
    u64 *pte = pgt[PIDX(level, va)];
    if(*pte & 1 == 0) {
      pgt = kalloc();
      *pte = PTE_PA(pgt) | 3;
    }
    else {
      pgt = PTE_PA(*pte);
    }
  }

  return pgt[PIDX(3, va)];
}

void page_map(u64 *pgt, u64 va, u64 pa, u64 size, u64 perm) {
  if(pa % PAGESIZE != 0 || size % PAGESIZE != 0)
    panic("invalid pa");

  for(u64 p = 0; p < size; p += PAGESIZE, va += size, pa += size) {
    u64 *pte = pagewalk(pgt, va);
    if(*pte & PTE_AF)
      panic("this entry has been used");
    *pte = PTE_PA(pa) | perm | 3;
  }
}

static void mmu_enable() {
  ;
}

u64 va2pa() {
  ;
}

void mm_init() {
  kpgt_init();
  upgt_init();

  u64 tcr = TCR_T0SZ(25) |  /* 2^(64-25) = 512GB */
            TCR_IRGN0(1) |
            TCR_ORGN0(1) |
            TCR_SH0(3) |
            TCR_TG0(0) |    /* TTBR0 granule 4KB */
            TCR_TG1(2) |    /* TTBR1 granule 4KB */
            TCR_T1SZ(25) |  /* 512GB */
            TCR_IPS(0) |    /* IPS = 4GB */
            TCR_EPD1(1);
  set_tcr_el1(tcr);

  isb();

  mmu_enable();
}
