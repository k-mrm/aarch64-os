#include "fs.h"
#include "dirent.h"

#define inode_nblock(i) ((i)->blocks / (2 << 0))

struct inode itable[NINODE];

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

char *inode_block(struct inode *ino, int bi) {
  if(bi < 12)
    return get_block(ino->block[bi]);
  else
    return get_indirect_block((u32 *)get_block(ino->block[12]), bi);

  return NULL;
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

static void free_inode(struct inode *ino) {
  memset(ino, 0, sizeof(*ino));
}

static void sync_inode(struct inode *ino) {
  ;
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

struct inode *new_inode(char *path, struct inode *dir, int mode, int major, int minor) {
  struct inode *ino = alloc_inode();
  if(!ino)
    return NULL;

  int inum = ext2_alloc_inum();
  if(inum < 0)
    return NULL;

  ino->inum = inum;
  ino->mode = mode;
  ino->major = major;
  ino->minor = minor;

  return ino;
}

bool isdir(struct inode *ino) {
  return ino->mode & S_IFDIR;
}

struct inode *path2i(char *path) {
  struct ext2_inode *eino = ext2_path2inode(path);
}

void fs_init() {
  ext2_init();

  memset(itable, 0, sizeof(struct inode) * NINODE);
}
