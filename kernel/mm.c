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
      pgt = (u64 *)P2V(PTE_PA(*pte));
    } else {
      pgt = kalloc();
      *pte = PTE_PA(V2P(pgt)) | PTE_TABLE | PTE_VALID;
    }
  }

  return &pgt[PIDX(3, va)];
}

u64 va2pa(u64 va) {
  kinfo("va2pa %p", va);
  u64 kpgtpa = ttbr1_el1();
  u64 *pte = pagewalk((u64 *)P2V(kpgtpa), va);

  return PTE_PA(*pte) + OFFSET(va);
}

u64 uva2pa(u64 *pgt, u64 va) {
  u64 *pte = pagewalk(pgt, va);

  return PTE_PA(*pte) + OFFSET(va);
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

static void pageunmap(u64 *pgt, u64 va, u64 size) {
  for(u64 p = 0; p < size; p += PAGESIZE, va += PAGESIZE) {
    u64 *pte = pagewalk(pgt, va);
    if(*pte == 0)
      panic("bad unmap");

    u64 pa = PTE_PA(*pte);

    kfree((void *)P2V(pa));

    *pte = 0;
  }
}

void free_table(u64 *pgt) {
  for(int i = 0; i < 512; i++) {
    u64 *pte = &pgt[i];

    if((*pte & PTE_VALID) && (*pte & PTE_TABLE) && !(*pte & PTE_AF)) {
      u64 *childpgt = (u64 *)PTE_PA(*pte);
      free_table((u64 *)P2V(childpgt));
      *pte = 0;
    }
  }
  kfree(pgt);
}


int alloc_userspace(u64 *pgt, u64 va, struct inode *ino, u64 srcoff, u64 size) {
  u64 pgsize;
  for(pgsize = PAGEROUNDUP(size); va < pgsize + va; va += PAGESIZE) {
    char *upage = kalloc();
    kinfo("map va %p to page %p\n", va, V2P(upage));
    read_inode(ino, upage, srcoff, size);
    kinfo("upage pa %p\n", va2pa(upage));
    pagemap(pgt, va, V2P(upage), PAGESIZE, PTE_NORMAL | PTE_U);
  }

  return pgsize;
}

void map_ustack(u64 *pgt) {
  char *ustack = kalloc();
  pagemap(pgt, USTACKBOTTOM, V2P(ustack), PAGESIZE, PTE_NORMAL | PTE_U | PTE_UXN | PTE_PXN);
}

void cp_userspace(u64 *newpgt, u64 *oldpgt, u64 size) {
  for(u64 va = 0; va < size; va += PAGESIZE) {
    u64 *pte = pagewalk(oldpgt, va);
    u64 pa = PTE_PA(*pte);
    char *page = kalloc();
    memcpy(page, (char *)P2V(pa), PAGESIZE);
    
    pagemap(newpgt, va, V2P(page), PAGESIZE, PTE_NORMAL | PTE_U);
  }
}

void free_userspace(u64 *pgt, u64 size) {
  kinfo("free userspace pgt %p size %d\n", pgt, size);
  pageunmap(pgt, 0, size);

  pageunmap(pgt, USTACKBOTTOM, PAGESIZE);

  free_table(pgt);
}

void load_userspace(u64 *pgt) {
  if(pgt == NULL)
    panic("no pagetable");
  kinfo("load pgt %p\n", pgt);

  set_ttbr0_el1(V2P(pgt));
}

void forget_userspace() {
  set_ttbr0_el1(0);
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
