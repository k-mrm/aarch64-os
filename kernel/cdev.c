#include "kernel.h"
#include "file.h"
#include "cdev.h"

struct cdevsw cdevsw[NCDEV];

void register_cdev(int major, struct cdevsw *cdev) {
  cdevsw[major].read = cdev->read;
  cdevsw[major].write = cdev->write;
}
