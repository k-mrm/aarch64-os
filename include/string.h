#ifndef AARCH64_OS_LIB_STRING_H
#define AARCH64_OS_LIB_STRING_H

#include "kernel.h"

void *memcpy(void *dst, const void *src, u64 n);
void *memmove(void *dst, const void *src, u64 n);
void *memset(void *dst, int c, u64 n);

#endif
