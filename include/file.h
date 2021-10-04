#ifndef AARCH64_OS_FILE_H
#define AARCH64_OS_FILE_H

#include "kernel.h"
#include "ext2.h"

struct dirent {
  u32 inode;
  u8 name_len;
  u8 file_type;
  char name[EXT2_DIRENT_NAME_MAX];
};

#endif
