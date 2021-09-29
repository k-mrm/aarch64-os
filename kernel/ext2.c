#include "ext2.h"
#include "printk.h"
#include "log.h"

struct imginfo imginfo;

void dump_superblock(struct superblock *sb) {
  printk("superblock dump sb %p\n", sb);
  printk("sizeof *sb %d\n", sizeof(*sb));
  printk("s_inodes_count: %d\n", sb->s_inodes_count);
  printk("s_blocks_count: %d\n", sb->s_blocks_count);
  printk("s_first_data_block: %d\n", sb->s_first_data_block);
  printk("s_last_mounted: %s\n", sb->s_last_mounted);
  printk("s_log_block_size: %d\n", sb->s_log_block_size);
  printk("s_mtime: %d\n", sb->s_mtime);
  printk("s_wtime: %d\n", sb->s_wtime);
  printk("s_state: %d\n", sb->s_state);
  printk("block_size: %d\n", 1024 << sb->s_log_block_size);
  printk("s_first_ino: %d\n", sb->s_first_ino);
  printk("s_inode_size: %d\n", sb->s_inode_size);
  printk("s_block_group_nr: %d\n", sb->s_block_group_nr);
  printk("s_blocks_per_group %d\n", sb->s_blocks_per_group);
  printk("s_inodes_per_group %d\n", sb->s_inodes_per_group);
}

void dump_bg_desc(struct bg_desc *bg) {
  printk("dump_bg_desc dump %p\n", bg);
  printk("sizeof *bg: %d\n", sizeof(*bg));
  printk("bg_block_bitmap: %d\n", bg->bg_block_bitmap);
  printk("bg_inode_bitmap: %d\n", bg->bg_inode_bitmap);
  printk("bg_inode_table: %d\n", bg->bg_inode_table);
  printk("bg_free_blocks_count: %d\n", bg->bg_free_blocks_count);
  printk("bg_free_inodes_count: %d\n", bg->bg_free_inodes_count);
  printk("bg_used_dirs_count: %d\n", bg->bg_used_dirs_count);
}

void fs_init(char *img) {
  struct superblock *sb = (struct superblock *)(img + 0x400);
  dump_superblock(sb);
  if(sb->s_magic != 0xef53)
    panic("invalid filesystem");
  u32 block_size = 1024 << sb->s_log_block_size;

  struct bg_desc *bg = (struct bd_desc *)(img + 0x800);
  dump_bg_desc(bg);
  kinfo("inode table %p\n", bg->bg_inode_table * block_size);
}
