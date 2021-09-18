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

  /*
  if(va == 0xffffff8047ffd000) {
    volatile char *a = (volatile char *)0xffffff8047ff5000;
    for(int i = 0; i < PAGESIZE; i++)
      printk("%p ", a[i]);
  }
  */

  memset(va, 0, PAGESIZE); /* bug on! */

  struct header *p = (struct header *)va;

  /*
  if(va == 0xffffff8047ffd000) {
    volatile char *a = (volatile char *)0xffffff8047ff5000;
    for(int i = 0; i < PAGESIZE; i++)
      printk("%p ", a[i]);
  }
  */

  p->next = freelist;
  freelist = p;
}

void kalloc_init1() {
  for(char *p = kend; p + PAGESIZE <= (char *)KERNSECEND; p += PAGESIZE) {
    kfree(p);
  }
}

void kalloc_init2() {
  for(char *p = (char *)KERNSECEND; p + PAGESIZE <= (char *)PHYMEMEND; p += PAGESIZE) {
    kfree(p);
  }
}
