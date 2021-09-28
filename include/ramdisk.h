#ifndef AARCH64_OS_RAMDISK_H
#define AARCH64_OS_RAMDISK_H

#include "kernel.h"

void ramdisk_init(void);
char *diskread(u64 off);

#endif
