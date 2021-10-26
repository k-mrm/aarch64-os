#include "kernel.h"
#include "ext2.h"
#include "dirent.h"
#include "printk.h"
#include "log.h"
#include "string.h"
#include "proc.h"
#include "kalloc.h"
#include "ramdisk.h"

struct superblock sb;

void dump_superblock(struct ext2_superblock *sb) {
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

void dump_bg_desc(struct ext2_bg_desc *bg) {
  printk("bg_desc dump %p\n", bg);
  printk("sizeof *bg: %d\n", sizeof(*bg));
  printk("bg_block_bitmap: %d\n", bg->bg_block_bitmap);
  printk("bg_inode_bitmap: %d\n", bg->bg_inode_bitmap);
  printk("bg_inode_table: %d\n", bg->bg_inode_table);
  printk("bg_free_blocks_count: %d\n", bg->bg_free_blocks_count);
  printk("bg_free_inodes_count: %d\n", bg->bg_free_inodes_count);
  printk("bg_used_dirs_count: %d\n", bg->bg_used_dirs_count);
}

void dump_inode(struct ext2_inode *i) {
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
  char *blk_end = blk + sb.bsize;
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
  char *blk_end = blk + sb.bsize;
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

static inline char *block_bitmap() {
  return sb.block_bitmap;
}

static inline char *inode_bitmap() {
  return sb.inode_bitmap;
}

static int find_free_ino(struct superblock *sb, char *bitmap) {
  char chunk = 0xff;
  int inum = 1;
  for(int i = 0; i < sb->block_size; i++) {
    if((chunk = bitmap[i]) != 0xff)
      break;
    inum += 8;
  }

  if(chunk == 0xff)   /* no free inode */
    return -1;

  for(; chunk & 1; chunk >>= 1)
    inum++;

  return inum;
}

static int find_free_block(char *bitmap) {
  char chunk = 0xff;
  int bnum = 0;
  for(int i = 0; i < sb.bsize; i++) {
    if((chunk = bitmap[i]) != 0xff)
      break;
    bnum += 8;
  }

  if(chunk == 0xff)   /* no free inode */
    return -1;

  for(; chunk & 1; chunk >>= 1)
    bnum++;

  return bnum;
}

static int ibmp_write_bit(char *bitmap, int ino, int val) {
  int bitn = ino - 1;
  int chunkn;
  char c;
  for(chunkn = 0; chunkn < sb.bsize; chunkn++) {
    if(bitn < 8)
      goto found;
    bitn -= 8;
  }

  /* invalid inode number */
  return -1;

found:
  c = bitmap[chunkn];
  if(val)
    c |= 1 << bitn;
  else
    c &= ~(char)(1 << bitn);
  bitmap[chunkn] = c;

  return 0;
}

struct ext2_inode *ext2_get_inode(int inum) {
  return (struct ext2_inode *)(sb.inode_table + (inum - 1) * sizeof(struct ext2_inode));
}

struct int ext2_alloc_inum() {
  int inum = find_free_ino(inode_bitmap());
  if(inum < 0)
    return -1;

  ibmp_write_bit(inode_bitmap(), inum, 1);

  return inum;
}

static int dirlink(struct inode *pdir, char *pname, struct inode *ino) {
}

static struct inode *new_inode(char *path, struct inode *dir, int mode, int major, int minor) {
  struct inode *ino = alloc_inode(mode, major, minor);
  if(!ino)
    return NULL;

  struct inode *pdir;

  dirlink(dir, path, ino);

  return ino;
}

static struct inode *ext2_mkreg(char *path, struct inode *cwd) {
  struct inode *ino = new_inode(path, cwd, EXT2_S_IFREG, 0, 0);
}

static struct inode *ext2_mkdir(char *path, struct inode *cwd) {
  struct inode *ino = new_inode(path, cwd, EXT2_S_IFDIR, 0, 0);
}

static struct inode *ext2_mkcdev(char *path, struct inode *cwd, int major, int minor) {
  struct inode *ino = new_inode(path, cwd, EXT2_S_IFCHR, major, minor);
}

static struct inode *ext2_mkbdev(char *path, struct inode *cwd, int major, int minor) {
  struct inode *ino = new_inode(path, cwd, EXT2_S_IFBLK, major, minor);
}

static void *ext2_get_block(int bnum) {
  return diskread((u64)bnum * sb.bsize);
}

static void *get_indirect_block(u32 *map, int bnum) {
  char idx = bnum - 12;
  return get_block(map[idx]);
}

void w_inode_block(struct inode *ino, int bi, char *blk) {
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
  u32 bsize = sb.bsize;
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

int w_inode(struct inode *ino, char *buf, u64 off, u64 size) {
  u32 bsize = ino->sb->bsize;
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

    memcpy(d + offblkoff, buf, cpsize);

    buf += cpsize;
    size = size > bsize? size - bsize : 0;
    offblkoff = 0;
  }
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

static struct ext2_inode *traverse_inode(struct ext2_inode *pi, char *path, char *name) {
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

  return traverse_inode(ext2_get_inode(inum), path, name);
}

struct ext2_inode *ext2_path2inode(char *path) {
  struct ext2_inode *ino;

  if(*path == '/')
    ino = ext2_get_inode(EXT2_ROOT_INO);
  else
    ino = curproc->cwd;

  char name[DIRENT_NAME_MAX] = {0};
  ino = traverse_inode(ino, path, name);

  return ino;
}

void ext2_init() {
  char *img = diskread(0);
  struct ext2_superblock *esb = (struct ext2_superblock *)(img + 0x400);
  // dump_superblock(sb);
  if(esb->s_magic != 0xef53)
    panic("invalid filesystem");

  sb.block_size = 1024 << esb->s_log_block_size;

  struct ext2_bg_desc *bg = (struct ext2_bg_desc *)(img + 0x800);

  sb.bsize = block_size;
  sb.block_bitmap = ext2_get_block(bg->bg_block_bitmap);
  sb.inode_bitmap = ext2_get_block(bg->bg_inode_bitmap);
  sb.inode_table = ext2_get_block(bg->bg_inode_table);
}
