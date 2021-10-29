#ifndef AARCH64_OS_CDEV_H
#define AARCH64_OS_CDEV_H

#include "file.h"

struct cdevsw {
  int (*read)(struct file *f, char *buf, u64 sz);
  int (*write)(struct file *f, char *buf, u64 sz);
};

#define NCDEV 32

#define CDEV_CONSOLE 0

extern struct cdevsw cdevsw[NCDEV];

void register_cdev(int major, struct cdevsw *cdev);

#endif
