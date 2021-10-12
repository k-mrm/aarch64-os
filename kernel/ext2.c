#include "kernel.h"
#include "ext2.h"
#include "dirent.h"
#include "printk.h"
#include "log.h"
#include "string.h"
#include "proc.h"
#include "kalloc.h"

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
  printk("bg_desc dump %p\n", bg);
  printk("sizeof *bg: %d\n", sizeof(*bg));
  printk("bg_block_bitmap: %d\n", bg->bg_block_bitmap);
  printk("bg_inode_bitmap: %d\n", bg->bg_inode_bitmap);
  printk("bg_inode_table: %d\n", bg->bg_inode_table);
  printk("bg_free_blocks_count: %d\n", bg->bg_free_blocks_count);
  printk("bg_free_inodes_count: %d\n", bg->bg_free_inodes_count);
  printk("bg_used_dirs_count: %d\n", bg->bg_used_dirs_count);
}

#define inode_nblock(i) ((i)->i_blocks / (2 << 0))

void dump_inode(struct inode *i) {
  printk("inode dump: %p\n", i);
  printk("sizeof *i: %d\n", sizeof(*i));
  printk("i_mode: %p\n", i->i_mode);
  printk("i_size: %d\n", i->i_size);
  printk("i_blocks: %d\n", i->i_blocks);
  for(int b = 0; b < 15; b++)
    printk("i_block[%d]: %p\n", b, i->i_block[b]);
}

void dump_dirent(struct dirent *d) {
  printk("dirent dump: %p\n", d);
  printk("sizeof *d: %d\n", sizeof(*d));
  printk("inode: %d\n", d->inode);
  printk("rec_len: %d\n", d->rec_len);
  printk("name_len: %d\n", d->name_len);
  printk("file_type: %d\n", d->file_type);
  printk("name: %s\n", d->name);
}

void dump_dirent_block(char *blk) {
  struct dirent *d = (struct dirent *)blk;
  char *blk_end = blk + imginfo.block_size;
  char *cd;

  while(d != blk_end && d->inode != 0) {
    dump_dirent(d);
    cd = (char *)d;
    cd += d->rec_len;
    d = (struct dirent *)cd;
  }
}

int search_dirent_block(char *blk, char *path) {
  struct dirent *d = (struct dirent *)blk;
  char *blk_end = blk + imginfo.block_size;
  char *cd;
  char buf[DIRENT_NAME_MAX];

  while(d != blk_end && d->inode != 0) {
    memset(buf, 0, DIRENT_NAME_MAX);
    memcpy(buf, d->name, d->name_len);
    if(strcmp(buf, path) == 0)
      return d->inode;

    cd = (char *)d;
    cd += d->rec_len;
    d = (struct dirent *)cd;
  }

  return -1;
}

static struct inode *get_inode(int inum) {
  return (struct inode *)(imginfo.inode_table + (inum - 1) * sizeof(struct inode));
}

static void *get_block(int bnum) {
  return imginfo.base + (u64)bnum * imginfo.block_size;
}

char *inode_block(struct inode *ino, int bi) {
  if(bi < 12)
    return get_block(ino->i_block[bi]);
  /* unimpl */
  return NULL;
}

void ls_inode(struct inode *ino) {
  if(ino == NULL) {
    printk("null inode\n");
    return;
  }
  if((ino->i_mode & EXT2_S_IFDIR) == 0) {
    printk("invalid inode: directory\n");
    return;
  }

  for(int i = 0; i < inode_nblock(ino); i++) {
    char *d = inode_block(ino, i);
    dump_dirent_block(d);
  }
}

int read_inode(struct inode *ino, char *buf, u64 off, u64 size) {
  // printk("readinode %p %d %d\n", buf, off, size);
  u32 bsize = imginfo.block_size;
  char *base = buf;

  if(off > ino->i_size)
    return -1;
  if(off + size > ino->i_size)
    size = ino->i_size - off;

  u32 offblk = off / bsize;
  u32 lastblk = (size + off) / bsize;
  u32 offblkoff = off % bsize;

  for(int i = offblk; i < inode_nblock(ino) && i <= lastblk; i++) {
    char *d = inode_block(ino, i);
    u64 cpsize = min(size, bsize);
    if(offblkoff + cpsize > bsize)
      cpsize = bsize - offblkoff;

    memcpy(buf, d + offblkoff, cpsize);

    buf += cpsize;
    size = size > bsize? size - bsize : 0;
    offblkoff = 0;
  }

  return buf - base;
}

static char *skippath(char *path, char *name, int *err) {
  /* skip '/' ("////aaa/bbb" -> "aaa/bbb") */
  while(*path == '/')
    path++;

  /* get elem and inc path (get "aaa/" from "aaa/bbb", path = "bbb") */
  int len = 0;
  while((*name = *path) && *path++ != '/') {
    name++;
    if(++len > DIRENT_NAME_MAX) {
      *err = 1;
      return NULL;
    }
  }

  /* cut '/' from name ("aaa/" -> "aaa") */
  if(*name == '/')
    *name = 0;
  return path;
}

static struct inode *traverse_inode(struct inode *pi, char *path, char *name) {
  int err = 0;
  path = skippath(path, name, &err);
  if(err)
    return NULL;
  if(*path == 0 && *name == 0)
    return pi;
  if((pi->i_mode & EXT2_S_IFDIR) == 0)
    return pi;

  int inum = -1;

  for(int i = 0; i < inode_nblock(pi); i++) {
    char *db = inode_block(pi, i);
    if((inum = search_dirent_block(db, name)) > 0)
      break;
  }

  if(inum < 0)
    return NULL;

  memset(name, 0, DIRENT_NAME_MAX);

  return traverse_inode(get_inode(inum), path, name);
}

struct inode *path2inode(char *path) {
  struct inode *ino;

  if(*path == '/')
    ino = get_inode(EXT2_ROOT_INO);
  else
    ino = curproc->cwd;

  char name[DIRENT_NAME_MAX] = {0};
  ino = traverse_inode(ino, path, name);

  return ino;
}

int ext2_getdents(struct inode *ino, struct dirent *dir, u64 count) {
  ;
}

void fs_init(char *img) {
  struct superblock *sb = (struct superblock *)(img + 0x400);
  // dump_superblock(sb);
  if(sb->s_magic != 0xef53)
    panic("invalid filesystem");
  u32 block_size = 1024 << sb->s_log_block_size;

  struct bg_desc *bg = (struct bg_desc *)(img + 0x800);
  // dump_bg_desc(bg);

  imginfo.base = img;
  imginfo.block_size = block_size;
  imginfo.block_bitmap = get_block(bg->bg_block_bitmap);
  imginfo.inode_bitmap = get_block(bg->bg_inode_bitmap);
  imginfo.inode_table = get_block(bg->bg_inode_table);
}
