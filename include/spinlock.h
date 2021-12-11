#ifndef AARCH64_OS_SPINLOCK_H
#define AARCH64_OS_SPINLOCK_H

#include "kernel.h"

struct spinlock {
  u8 locked;
  int cpuid;
};

void acquire(struct spinlock *lk);
void release(struct spinlock *lk);
void lock_init(struct spinlock *lk);

#endif