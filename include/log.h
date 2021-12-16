#ifndef AARCH64_OS_LOG_H
#define AARCH64_OS_LOG_H

#include "printk.h"

#ifdef OS_DEBUG

#define kinfo(...)  do { printk("cpu%d @%s", cpuid(), __func__); printk("[INFO] " __VA_ARGS__); } while(0)

#else

#define kinfo(...)  (void)(0)

#endif

#endif
