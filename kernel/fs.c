#include "fs.h"
#include "ext2.h"
#include "dirent.h"

struct inode itable[NINODE];

void dump_dirent(struct dirent *d) {
  printk("dirent dump: %p\n", d);
  printk("sizeof *d: %d\n", sizeof(*d));
  printk("inode: %d\n", d->inode);
  printk("rec_len: %d\n", d->rec_len);
  printk("name_len: %d\n", d->name_len);
  printk("file_type: %d\n", d->file_type);
  printk("name: %s\n", d->name);
}

static struct inode *load_dinode(struct ext2_inode *e, struct inode *i) {
  i->mode = e->i_mode;
  i->uid = e->i_uid;
  i->size = e->i_size;
  i->atime = e->i_atime;
  i->ctime = e->i_ctime;
  i->mtime = e->i_mtime;
  i->dtime = e->i_dtime;
  i->gid = e->i_gid;
  i->links_count = e->i_links_count;
  i->blocks = e->i_blocks;
  i->flags = e->i_flags;
  memcpy(i->block, e->i_block, sizeof(u32) * 15);

  return i;
}

static struct inode *alloc_inode() {
  struct inode *ino;
  for(int i = 0; i < NINODE; i++) {
    ino = &itable[i];
    if(ino->mode == 0)
      return ino;
  }

  return NULL;
}

static struct inode *get_inode(int inum) {
  struct inode *ino;
  for(int i = 0; i < NINODE; i++) {
    ino = &itable[i];
    if(ino->inum == inum)
      return ino;
  }

  ino = alloc_inode();
  if(!ino)
    return NULL;

  struct ext2_inode *eino = ext2_get_inode(inum);

  load_dinode(eino, ino, inum);

  ino->inum = inum;

  return ino;
}

bool isdir(struct inode *ino) {
  return ino->mode & S_IFDIR;
}

struct inode *path2i(char *path) {
  struct ext2_inode *eino = ext2_path2inode(path);
}

void fs_init(char *img) {
  ext2_init(img);

  memset(itable, 0, sizeof(struct inode) * NINODE);
}
