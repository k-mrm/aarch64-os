/*
 *  physical memory allocator(4KB)
 */

#include "kernel.h"
#include "memmap.h"
#include "mm.h"
#include "aarch64.h"
#include "kalloc.h"
#include "printk.h"
#include "string.h"
#include "log.h"
#include "spinlock.h"

extern char kend[];

struct header {
  struct header *next;
};

struct kallocator {
  struct spinlock lk;
  struct header *freelist;
} kallocator;

void *kalloc() {
  acquire(&kallocator.lk);

  struct header *new = kallocator.freelist;
  if(!new)  /* no memory or uninitialized */
    return NULL;

  kallocator.freelist = new->next;

  release(&kallocator.lk);

  memset((char *)new, 0, PAGESIZE);

  return (void *)new;
}

void kfree(void *va) {
  if(va == NULL)
    return;
  if((u64)va % PAGESIZE != 0)
    panic("bad va");

  memset(va, 0, PAGESIZE);

  struct header *p = (struct header *)va;

  acquire(&kallocator.lk);

  p->next = kallocator.freelist;
  kallocator.freelist = p;

  release(&kallocator.lk);
}

u64 freed_mem_kb() {
  acquire(&kallocator.lk);

  struct header *h = kallocator.freelist;

  int c = 0;
  for(; h; h = h->next)
    c++;

  release(&kallocator.lk);

  return c * PAGESIZE / 1024;
}

static inline u64 ksecend() {
  return (((u64)kend + SECTIONSIZE - 1) & ~(SECTIONSIZE - 1));
}

void kalloc_init1() {
  lock_init(&kallocator.lk);
  kallocator.freelist = NULL;

  for(char *p = kend; p + PAGESIZE <= (char *)ksecend(); p += PAGESIZE)
    kfree(p);
}

void kalloc_init2() {
  for(char *p = (char *)ksecend(); p + PAGESIZE <= (char *)PHYMEMEND; p += PAGESIZE)
    kfree(p);
}
