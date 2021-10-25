#ifndef AARCH64_OS_DIRENT_H
#define AARCH64_OS_DIRENT_H

#include "kernel.h"

#define T_UNKNOWN   EXT2_FT_UNKNOWN
#define T_REG_FILE  EXT2_FT_REG_FILE
#define T_DIR       EXT2_FT_DIR
#define T_CHRDEV    EXT2_FT_CHRDEV
#define T_BLKDEV    EXT2_FT_BLKDEV
#define T_FIFO      EXT2_FT_FIFO
#define T_SOCK      EXT2_FT_SOCK
#define T_SYMLINK   EXT2_FT_SYMLINK

struct dirent {
  u32 inode;
  u16 rec_len;
  u8 name_len;
  u8 file_type;
  char name[1];
} __attribute__((packed));

#define DIRENT_NAME_MAX 255

#endif
