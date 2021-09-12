#include "kernel.h"
#include "aarch64.h"
#include "mm.h"
#include "memmap.h"
#include "kalloc.h"
#include "printk.h"
#include "log.h"
#include "string.h"

extern u64 l1kpgt[];
extern char kend[];

static u64 *pagewalk(u64 *pgt, u64 va) {
  for(int level = 1; level < 3; level++) {
    u64 *pte = &pgt[PIDX(level, va)];
    if((*pte & PTE_VALID) && (*pte & PTE_TABLE)) {
      pgt = (u64 *)PTE_PA(*pte);
    } else {
      pgt = kalloc();
      *pte = PTE_PA(V2P(pgt)) | PTE_TABLE | PTE_VALID;
    }
  }

  return &pgt[PIDX(3, va)];
}

static void pagemap(u64 *pgt, u64 va, u64 pa, u64 size, u64 attr) {
  if(pa % PAGESIZE != 0 || size % PAGESIZE != 0)
    panic("invalid pa");

  for(u64 p = 0; p < size; p += PAGESIZE, va += PAGESIZE, pa += PAGESIZE) {
    u64 *pte = pagewalk(pgt, va);
    if(*pte & PTE_AF)
      panic("this entry has been used");
    *pte = PTE_PA(pa) | PTE_AF | attr | PTE_V;
  }
}
void alloc_userspace(u64 *pgt, u64 begin, u64 size) {
  /* map usr_begin ~ usr_end to 0 ~  */
  for(u64 va = 0; va < size; va += PAGESIZE) {
    char *upage = kalloc();
    memcpy(upage, (char *)begin, PAGESIZE);
    pagemap(pgt, va, V2P(upage), PAGESIZE, PTE_NORMAL | PTE_U);
  }

  /* map usr stack */
  char *ustack = kalloc();
  pagemap(pgt, USTACKTOP - PAGESIZE, V2P(ustack), PAGESIZE, PTE_NORMAL | PTE_U | PTE_UXN | PTE_PXN);
}

void load_userspace(u64 *pgt) {
  if(pgt == NULL)
    panic("no pagetable");

  printk("pgttttt %p %p\n", pgt, V2P(pgt));
  set_ttbr0_el1(V2P(pgt));
}

u64 va2pa() {
  return 0;
}

void kpgt_init() {
  /* remap kernel */
  pagemap(l1kpgt, KERNBASE, PKERNBASE, (u64)kend - KERNBASE, PTE_NORMAL);

  /* map kend ~ PHYMEMEND */
  pagemap(l1kpgt, (u64)kend, V2P(kend), PHYMEMEND - (u64)kend, PTE_NORMAL);
}

void pgt_init() {
  kpgt_init();

  isb();

  set_ttbr0_el1(0);
}
