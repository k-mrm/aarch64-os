#ifndef AARCH64_OS_DIRENT_H
#define AARCH64_OS_DIRENT_H

#include "kernel.h"

#define DT_UNKNOWN  0
#define DT_REG      1
#define DT_DIR      2
#define DT_CHR      3
#define DT_BLK      4
#define DT_FIFO     5
#define DT_SOCK     6
#define DT_LNK      7

struct dirent {
  u32 inode;
  u16 rec_len;
  u8 name_len;
  u8 file_type;
  char name[];
} __attribute__((packed));

#define DIRENT_NAME_MAX 255

#endif
