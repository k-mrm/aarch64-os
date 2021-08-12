#ifndef MONO_KALLOC_H
#define MONO_KALLOC_H

#define PAGESIZE  4096

void *kalloc(void);
void kfree(void *pa);
void kalloc_init(void);
void kalloctest(void);

#endif
