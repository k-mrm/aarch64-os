#include "ext2.h"

void dump_superblock(struct superblock *sb) {
  printk("superblock dump sb %p\n", sb);
  printk("s_inodes_count: %d\n", sb->s_inodes_count);
  printk("s_blocks_count: %d\n", sb->s_blocks_count);
  printk("s_last_mounted: %s\n", sb->s_last_mounted);
}
