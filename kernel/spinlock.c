#include "aarch64.h"
#include "spinlock.h"
#include "printk.h"

bool holding(struct spinlock *lk) {
  return lk->locked && lk->cpuid == cpuid();
}

void acquire(struct spinlock *lk) {
  if(holding(lk))
    panic("already held");

  /* TODO */
  while(__sync_lock_test_and_set(&lk->locked, 1) != 0)
    ;

  isb();

  lk->cpuid = cpuid();
}

void release(struct spinlock *lk) {
  if(!holding(lk))
    panic("invalid release");

  lk->cpuid = -1;

  isb();

  asm volatile("str wzr, %0" : "=m"(lk->locked));
}

