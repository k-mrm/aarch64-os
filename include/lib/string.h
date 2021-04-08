#ifndef MONO_LIB_STRING_H
#define MONO_LIB_STRING_H

#include "mono.h"

void *memcpy(void *dst, const void *src, u64 n);
void *memmove(void *dst, const void *src, u64 n);
void *memset(void *dst, int c, u64 n);

#endif
