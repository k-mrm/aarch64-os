#ifndef AARCH64_OS_DIRENT_H
#define AARCH64_OS_DIRENT_H

#include "kernel.h"

#define DT_UNKNOWN  EXT2_FT_UNKNOWN
#define DT_REG      EXT2_FT_REG_FILE
#define DT_DIR      EXT2_FT_DIR
#define DT_CHR      EXT2_FT_CHRDEV
#define DT_BLK      EXT2_FT_BLKDEV
#define DT_FIFO     EXT2_FT_FIFO
#define DT_SOCK     EXT2_FT_SOCK
#define DT_LNK      EXT2_FT_SYMLINK

struct dirent {
  u32 inode;
  u16 rec_len;
  u8 name_len;
  u8 file_type;
  char name[1];
} __attribute__((packed));

#define DIRENT_NAME_MAX 255

#endif
