#include "kernel.h"
#include "fs.h"
#include "dirent.h"
#include "string.h"
#include "ext2.h"

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

static void free_inode(struct inode *ino) {
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

struct inode *new_inode(char *path, struct inode *dir, int mode, int major, int minor) {
  ;
}

struct inode *fs_mknod(const char *path, int dev) {
  ;
}

struct inode *fs_mkdir(const char *path) {
  ;
}

struct inode *path2inode(char *path) {
  struct inode *ino = ext2_path2inode(path);
  return ino;
}

void fs_init() {
  ext2_init();

  memset(itable, 0, sizeof(struct inode) * NINODE);
}
