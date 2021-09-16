#ifndef AARCH64_OS_USR_USYS_H
#define AARCH64_OS_USR_USYS_H

#include "ulib.h"

int getpid(void);
int write(char *s, u64 size);
int exit(int ret);

#endif
