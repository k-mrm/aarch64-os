#ifndef AARCH64_OS_DIRENT_H
#define AARCH64_OS_DIRENT_H

#include "kernel.h"

struct dirent {
  u32 inode;
  u16 rec_len;
  u8 name_len;
  u8 file_type;
  char name[1];
} __attribute__((packed));

#define DIRENT_NAME_MAX 255

#endif
