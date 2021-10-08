#ifndef AARCH64_OS_FILE_H
#define AARCH64_OS_FILE_H

#include "kernel.h"
#include "ext2.h"

#define NFILE 128

struct file {
  int ref;
  struct inode *ino;
  u64 off;
};

extern struct file ftable[];

struct cdevsw {
  int (*read)(void);
  int (*write)(void);
};

void file_init(void);

#endif
