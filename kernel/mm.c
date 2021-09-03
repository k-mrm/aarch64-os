#include "mono.h"
#include "aarch64.h"
#include "mm.h"
#include "memmap.h"
#include "kalloc.h"
#include "printk.h"
#include "log.h"

extern u64 l1kpgt[];
extern char kend[];

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

static u64 *kpagewalk(u64 *pgt, u64 va) {
  for(int level = 1; level < 3; level++) {
    u64 *pte = (u64 *)pgt[PIDX(level, va)];
    if((*pte & PTE_VALID) && (*pte & PTE_TABLE)) {
      pgt = (u64 *)PTE_PA(*pte);
    } else {
      pgt = kalloc();
      *pte = PTE_PA(pgt) | PTE_TABLE | PTE_VALID;
    }
  }

  return (u64 *)pgt[PIDX(3, va)];
}

static void kpagemap(u64 *pgt, u64 va, u64 pa, u64 size, u64 attr) {
  if(pa % PAGESIZE != 0 || size % PAGESIZE != 0)
    panic("invalid pa");

  for(u64 p = 0; p < size; p += PAGESIZE, va += PAGESIZE, pa += PAGESIZE) {
    u64 *pte = kpagewalk(pgt, va);
    if(*pte & PTE_AF)
      panic("this entry has been used");
    *pte = PTE_PA(pa) | attr | PTE_TABLE | PTE_VALID;
  }
}

static void pagemap(u64 *pgt, u64 va, u64 pa, u64 size, u64 attr) {
  if(pa % PAGESIZE != 0 || size % PAGESIZE != 0)
    panic("invalid pa");

  for(u64 p = 0; p < size; p += PAGESIZE, va += PAGESIZE, pa += PAGESIZE) {
    u64 *pte = pagewalk(pgt, va);
    if(*pte & PTE_AF)
      panic("this entry has been used");
    *pte = PTE_PA(pa) | attr | PTE_TABLE | PTE_VALID;
  }
}

u64 va2pa() {
  return 0;
}

void kpgt_init() {
  kinfo("kpgt %p\n", l1kpgt);
  /* remap kernel */
  kpagemap(l1kpgt, KERNBASE, PKERNBASE, (u64)kend - KERNBASE, PTE_INDX(AI_NORMAL_NC_IDX));

  /* map kend ~ PHYMEMEND */
  kpagemap(l1kpgt, (u64)kend, V2P((u64)kend), PHYMEMEND - (u64)kend, PTE_INDX(AI_NORMAL_NC_IDX));

  kinfo("a\n");
}

void pgt_init() {
  kpgt_init();

  set_ttbr0_el1(0);
}
