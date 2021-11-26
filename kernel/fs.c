#include "kernel.h"
#include "fs.h"
#include "dirent.h"
#include "string.h"
#include "ext2.h"
#include "printk.h"

struct inode itable[NINODE];

struct superblock sb;

struct inode *alloc_inode() {
  struct inode *ino;
  for(int i = 0; i < NINODE; i++) {
    ino = &itable[i];
    if(ino->mode == 0)
      return ino;
  }

  return NULL;
}

void free_inode(struct inode *ino) {
  memset(ino, 0, sizeof(*ino));
}

struct inode *find_inode(int inum) {
  struct inode *ino;
  for(int i = 0; i < NINODE; i++) {
    ino = &itable[i];
    if(ino->inum == inum)
      return ino;
  }

  ino = alloc_inode();
  if(!ino)
    return NULL;

  ino->inum = inum;

  return ino;
}

struct inode *get_inode(int inum) {
  return ext2_get_inode(inum);
}

int read_inode(struct inode *ino, char *buf, u64 off, u64 size) {
  return ext2_read_inode(ino, buf, off, size);
}

int write_inode(struct inode *ino, char *buf, u64 off, u64 size) {
  return ext2_w_inode(ino, buf, off, size);
}

struct inode *fs_mknod(char *path, int mode, int dev) {
  return ext2_mknod(path, mode, dev);
}

struct inode *fs_mkdir(char *path) {
  return ext2_mkdir(path);
}

struct inode *path2inode(char *path) {
  struct inode *ino = ext2_path2inode(path);
  return ino;
}

void dump_inode(struct inode *i) {
  if(!i)
    printk("dump inode???????????????\n");

  printk("inode dump: %p\n", i);
  printk("sizeof inode: %d\n", sizeof(*i));
  printk("inum: %d\n", i->inum);
  printk("i_mode: %p\n", i->mode);
  printk("i_links_count: %p\n", i->links_count);
  printk("i_size: %d\n", i->size);
  printk("i_blocks: %d\n", i->blocks);
  for(int b = 0; b < 15; b++)
    printk("i_block[%d]: %x\n", b, i->block[b]);
}

void fs_init() {
  ext2_init();

  memset(itable, 0, sizeof(struct inode) * NINODE);
}

