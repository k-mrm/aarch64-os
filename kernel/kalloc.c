#include "mono.h"
#include "aarch64.h"
#include "kalloc.h"

/*
 *  physical memory allocator
 */

extern char *ktext_end;

void *allocpage() {
  static int i = 0;
  return (void *)(ktext_end + (i++) * PAGESIZE);
}
