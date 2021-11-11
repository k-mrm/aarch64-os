#include "kernel.h"
#include "memmap.h"
#include "mm.h"
#include "aarch64.h"
#include "kalloc.h"
#include "printk.h"
#include "string.h"
#include "log.h"

/*
 *  physical memory allocator
 */

extern char kend[];

struct header {
  struct header *next;
};

struct header *freelist = NULL;

void *kalloc() {
  struct header *new = freelist;
  if(!new)  /* no memory or uninitialized */
    return NULL;

  freelist = new->next;

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
  p->next = freelist;
  freelist = p;
}

static inline u64 ksecend() {
  return (((u64)kend + SECTIONSIZE - 1) & ~(SECTIONSIZE - 1));
}

void kalloc_init1() {
  for(char *p = kend; p + PAGESIZE <= (char *)ksecend(); p += PAGESIZE) {
    kfree(p);
  }
}

void kalloc_init2() {
  for(char *p = (char *)ksecend(); p + PAGESIZE <= (char *)PHYMEMEND; p += PAGESIZE) {
    kfree(p);
  }
}
