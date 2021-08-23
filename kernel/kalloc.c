#include "mono.h"
#include "memmap.h"
#include "aarch64.h"
#include "kalloc.h"
#include "printk.h"
#include "string.h"

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

void kfree(void *pa) {
  if(!pa)
    return;
  printk("%p\n", pa);

  if((u64)pa % PAGESIZE != 0) {
    panic("a");
  }

  struct header *p = (struct header *)pa;
  p->next = freelist;
  freelist = p;

  memset((char *)pa, 0, PAGESIZE);
}

void kalloc_init() {
  for(char *p = kend; p + PAGESIZE <= (char *)PHYMEM_END; p += PAGESIZE) {
    struct header *ph = (struct header *)p;
    ph->next = freelist;
    freelist = ph;
  }
}

void kalloctest() {
  char *p1 = kalloc();
  char *p2 = kalloc();
  char *p3 = kalloc();
  kfree(p1);
  char *p4 = kalloc();
  printk("%d\n", *p1);
}
