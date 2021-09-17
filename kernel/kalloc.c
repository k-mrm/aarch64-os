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

  struct header *p = (struct header *)va;
  p->next = freelist;
  freelist = p;

  // memset((char *)va, 0, PAGESIZE);
}

void kalloc_init1() {
  for(char *p = kend; p + PAGESIZE <= (char *)KERNSECEND; p += PAGESIZE) {
    struct header *ph = (struct header *)p;
    ph->next = freelist;
    freelist = ph;
  }
}

void kalloc_init2() {
  for(char *p = (char *)KERNSECEND; p + PAGESIZE <= (char *)PHYMEMEND; p += PAGESIZE) {
    struct header *ph = (struct header *)p;
    ph->next = freelist;
    freelist = ph;
  }
}
