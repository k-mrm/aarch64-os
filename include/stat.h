#ifndef AARCH64_OS_STAT_H
#define AARCH64_OS_STAT_H

#include "kernel.h"
#include "fs.h"

struct stat {
  int st_dev;
  u32 st_ino;
  u16 st_mode;
  u32 st_size;
  u32 st_nlink;
  u32 st_blksize;
};

#endif
