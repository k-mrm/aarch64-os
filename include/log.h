#ifndef AARCH64_OS_LOG_H
#define AARCH64_OS_LOG_H

#include "aarch64.h"
#include "printk.h"

#ifdef OS_DEBUG

#define kinfo(...)  \
  do { printk("[INFO] cpu%d @%s:\t", cpuid(), __func__); printk(__VA_ARGS__); } while(0)

#else

#define kinfo(...)

#endif

#endif
