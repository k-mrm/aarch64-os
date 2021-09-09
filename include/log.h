#ifndef AARCH64_OS_LOG_H
#define AARCH64_OS_LOG_H

#include "printk.h"

#define kinfo(...)  printk("[INFO] " __VA_ARGS__)    

#endif
