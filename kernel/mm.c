#include "mono.h"
#include "aarch64.h"
#include "mm.h"
#include "kalloc.h"
#include "printk.h"

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

static u64 *pagewalk(u64 *pgt, u64 va) {
  for(int level = 1; level < 3; level++) {
    u64 *pte = (u64 *)pgt[PIDX(level, va)];
    if((*pte & 1) == 0) {
      pgt = kalloc();
      *pte = PTE_PA(pgt) | PTE_TABLE | PTE_VALID;
    } else {
      pgt = (u64 *)PTE_PA(*pte);
    }
  }

  return (u64 *)pgt[PIDX(3, va)];
}

static void pagemap(u64 *pgt, u64 va, u64 pa, u64 size, u64 attr) {
  if(pa % PAGESIZE != 0 || size % PAGESIZE != 0)
    panic("invalid pa");

  for(u64 p = 0; p < size; p += PAGESIZE, va += size, pa += size) {
    u64 *pte = pagewalk(pgt, va);
    if(*pte & PTE_AF)
      panic("this entry has been used");
    *pte = PTE_PA(pa) | attr | PTE_TABLE | PTE_VALID;
  }
}

static void enable_mmu() {
  ;
}

u64 va2pa() {
  return 0;
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
            TCR_IPS(0);    /* IPS = 4GB */
  set_tcr_el1(tcr);

  isb();

  enable_mmu();
}
