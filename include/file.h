#ifndef AARCH64_OS_FILE_H
#define AARCH64_OS_FILE_H

#include "kernel.h"
#include "fs.h"

#define NFILE 128

struct file {
  int ref;
  struct inode *ino;
  u64 off;
  int readable;
  int writable;
};

void file_init(void);
struct file *dup_file(struct file *f);

#endif
