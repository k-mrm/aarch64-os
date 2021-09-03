#ifndef MONO_KALLOC_H
#define MONO_KALLOC_H

void *kalloc(void);
void kfree(void *pa);
void kalloc_init1(void);
void kalloc_init2(void);
void kalloctest(void);

#endif
