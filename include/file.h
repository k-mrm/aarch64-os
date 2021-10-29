#ifndef AARCH64_OS_FILE_H
#define AARCH64_OS_FILE_H

#include "kernel.h"
#include "fs.h"

#define NFILE 128

enum ftype {
  F_NONE,
  F_INODE,
  F_CDEV,
};

struct file {
  enum ftype type;
  int ref;
  struct inode *ino;
  u64 off;
  int readable;
  int writable;
};

extern struct file ftable[];

void file_init(void);

#endif
