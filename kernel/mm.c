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
      if(!pgt) {
        printk("null pgt\n");
        return NULL;
      }

      *pte = PTE_PA(V2P(pgt)) | PTE_TABLE | PTE_VALID;
    }
  }

  return &pgt[PIDX(3, va)];
}

u64 va2pa(u64 va) {
  u64 kpgtpa = ttbr1_el1();
  u64 *pte = pagewalk((u64 *)P2V(kpgtpa), va);
  if(!pte)
    return 0;

  return PTE_PA(*pte) + OFFSET(va);
}

u64 uva2pa(u64 *pgt, u64 va) {
  u64 *pte = pagewalk(pgt, va);
  if(!pte)
    return 0;

  return PTE_PA(*pte) + OFFSET(va);
}

u64 uva2ka(u64 *pgt, u64 va) {
  u64 pa = uva2pa(pgt, va);
  if(!pa)
    return 0;

  return (u64)P2V(pa);
}

static void pagemap(u64 *pgt, u64 va, u64 pa, u64 size, u64 attr) {
  if(va % PAGESIZE != 0 || pa % PAGESIZE != 0 || size % PAGESIZE != 0)
    panic("invalid pagemap");

  for(u64 p = 0; p < size; p += PAGESIZE, va += PAGESIZE, pa += PAGESIZE) {
    u64 *pte = pagewalk(pgt, va);
    if(*pte & PTE_AF)
      panic("this entry has been used");
    *pte = PTE_PA(pa) | PTE_AF | attr | PTE_V;
  }
}

static void pageunmap(u64 *pgt, u64 va, u64 size) {
  if(va % PAGESIZE != 0 || size % PAGESIZE != 0)
    panic("invalid pageunmap");

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

void init_userspace(u64 *pgt, u64 va, char *code, u64 size) {
  u64 pgsize = PAGEROUNDUP(size);
  for(u64 p = 0; p < pgsize; p += PAGESIZE, va += PAGESIZE) {
    char *upage = kalloc();
    if(!upage)
      panic("init_userspace");
    memcpy(upage, (char *)code, size);
    pagemap(pgt, va, V2P(upage), PAGESIZE, PTE_NORMAL | PTE_U);
  }
}

int alloc_userspace(u64 *pgt, u64 va, struct inode *ino, u64 srcoff, u64 size) {
  u64 pgsize = PAGEROUNDUP(size);
  for(u64 p = 0; p < pgsize; p += PAGESIZE, va += PAGESIZE) {
    char *upage = kalloc();
    if(!upage)
      return -1;
    kinfo("map va %p to page %p\n", va, V2P(upage));
    read_inode(ino, upage, srcoff, size);
    kinfo("upage pa %p\n", va2pa((u64)upage));
    pagemap(pgt, va, V2P(upage), PAGESIZE, PTE_NORMAL | PTE_U);
  }

  return pgsize;
}

void *grow_userspace(u64 *pgt, u64 va, u64 oldsz, u64 newsz) {
  u64 start = va + oldsz;
  oldsz = PAGEROUNDUP(oldsz);
  va += oldsz;
  for(u64 p = oldsz; p < newsz; p += PAGESIZE, va += PAGESIZE) {
    char *upage = kalloc();
    if(!upage)
      return NULL;
    pagemap(pgt, va, V2P(upage), PAGESIZE, PTE_NORMAL | PTE_U);
  }
  kinfo("start %p va %p %d %d\n", start, va, oldsz, newsz);

  return (void *)start;
}

void *shrink_userspace(u64 *pgt, u64 va, u64 oldsz, u64 newsz) {
}

char *map_ustack(u64 *pgt) {
  char *ustack = kalloc();
  if(!ustack)
    return NULL;

  pagemap(pgt, USTACKBOTTOM, V2P(ustack), PAGESIZE, PTE_NORMAL | PTE_U | PTE_UXN | PTE_PXN);

  return ustack;
}

void dump_ustack(u64 *pgt) {
  char *page = (char *)P2V(uva2pa(pgt, USTACKBOTTOM));
  kinfo("dump ustack %p\n", page);

  for(int i = 0; i < PAGESIZE; i++)
    printk("%x ", page[i]);
}

int cp_userspace(u64 *newpgt, u64 *oldpgt, u64 va, u64 size) {
  u64 *pte;
  u64 pa;

  for(u64 p = 0; p < size; p += PAGESIZE, va += PAGESIZE) {
    pte = pagewalk(oldpgt, va);
    pa = PTE_PA(*pte);
    char *page = kalloc();
    if(!page)
      return -1;
    memcpy(page, (char *)P2V(pa), PAGESIZE);
    
    pagemap(newpgt, va, V2P(page), PAGESIZE, PTE_NORMAL | PTE_U);
  }

  /* copy ustack */
  pte = pagewalk(oldpgt, USTACKBOTTOM);
  pa = PTE_PA(*pte);
  char *newstack = kalloc();
  if(!newstack)
    return -1;
  memcpy(newstack, (char *)P2V(pa), PAGESIZE);

  pagemap(newpgt, USTACKBOTTOM, V2P(newstack), PAGESIZE, PTE_NORMAL | PTE_U | PTE_UXN | PTE_PXN);

  return 0;
}

void free_userspace(u64 *pgt, u64 size) {
  kinfo("free userspace pgt %p size %d\n", pgt, size);

  pageunmap(pgt, 0x1000, PAGEROUNDUP(size));
  pageunmap(pgt, USTACKBOTTOM, PAGESIZE);

  free_table(pgt);
}

void load_userspace(u64 *pgt) {
  if(pgt == NULL)
    panic("no pagetable");

  set_ttbr0_el1(V2P(pgt));
  flush_tlb();
}

void forget_userspace() {
  set_ttbr0_el1(0);
  flush_tlb();
}

void kpgt_init() {
  /* remap kernel */
  pagemap(l1kpgt, KERNBASE, PKERNBASE, (u64)kend - KERNBASE, PTE_NORMAL);
  /* map kend ~ PHYMEMEND */
  pagemap(l1kpgt, (u64)kend, V2P(kend), PHYMEMEND - (u64)kend, PTE_NORMAL);
}

void pgt_init() {
  set_ttbr1_el1(V2P(l1kpgt));
  set_ttbr0_el1(0);
  flush_tlb();
}
