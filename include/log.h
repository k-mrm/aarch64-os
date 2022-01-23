#ifndef AARCH64_OS_LOG_H
#define AARCH64_OS_LOG_H

#include "aarch64.h"
#include "printk.h"
#include "spinlock.h"

#ifdef KDBG

extern struct spinlock kinfo_lk;

#define kinfo(...)  \
  do { acquire(&kinfo_lk); printk("[INFO] cpu%d @%s:\t", cpuid(), __func__); printk(__VA_ARGS__); release(&kinfo_lk); } while(0)

#else

#define kinfo(...)

#endif

#endif
