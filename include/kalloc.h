#ifndef AARCH64_OS_KALLOC_H
#define AARCH64_OS_KALLOC_H

#include "kernel.h"

void *kalloc(void);
void kfree(void *pa);
void kalloc_init1(void);
void kalloc_init2(void);
u64 freed_mem_kb(void);

#endif
