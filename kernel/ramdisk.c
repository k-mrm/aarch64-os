#include "kernel.h"

extern char _binary_fs_img_start[];
extern char _binary_fs_img_end[];
extern char _binary_fs_img_size[];

struct {
  char *data;
  u64 size;
} disk;

void ramdisk_init() {
  disk.data = _binary_fs_img_start;
  disk.size = _binary_fs_img_size;
}

char *diskread(u64 off) {
  printk("disk %p\n", disk.data);
  return disk.data + off;
}
