#ifndef AARCH64_OS_FILE_H
#define AARCH64_OS_FILE_H

#include "aarch64-os.h"

struct file {
  int ino;
  char *path;
  char content[2048];
};

#endif
