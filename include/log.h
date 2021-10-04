#ifndef AARCH64_OS_LOG_H
#define AARCH64_OS_LOG_H

#include "printk.h"

#ifdef OS_DEBUG

#define kinfo(...)  printk("[INFO] " __VA_ARGS__)    

#else

#define kinfo(...)  (void)(0)

#endif

#endif
