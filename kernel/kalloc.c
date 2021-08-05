#include "mono.h"
#include "aarch64.h"
#include "kalloc.h"
#include "printk.h"

/*
 *  physical memory allocator
 */

extern char kend[];

void *allocpage() {
  static int i = 0;
  return (void *)(kend + (i++) * PAGESIZE);
}
