#include "kernel.h"
#include "ext2.h"
#include "dirent.h"
#include "printk.h"
#include "log.h"
#include "string.h"
#include "proc.h"
#include "kalloc.h"
#include "buf.h"

#define ext2_inode_nblock(i) ((i)->blocks / (2 << 0))

#define __unused __attribute__((unused))

static void dump_superblock(struct ext2_superblock *sb) __unused;
static void dump_bg_desc(struct ext2_bg_desc *bg) __unused;
static void dump_ext2_inode(struct ext2_inode *i) __unused;
static void dump_dirent(struct dirent *d) __unused;

static void inode_sync(struct inode *i);

struct inode *ext2_path2inode(char *path);
struct inode *ext2_path2inode_parent(char *path, char *namebuf);

static int ext2_search_dir(struct inode *dir, char *name);

static void dump_superblock(struct ext2_superblock *sb) {
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

static void dump_bg_desc(struct ext2_bg_desc *bg) {
  printk("bg_desc dump %p\n", bg);
  printk("sizeof *bg: %d\n", sizeof(*bg));
  printk("bg_block_bitmap: %d\n", bg->bg_block_bitmap);
  printk("bg_inode_bitmap: %d\n", bg->bg_inode_bitmap);
  printk("bg_inode_table: %d\n", bg->bg_inode_table);
  printk("bg_free_blocks_count: %d\n", bg->bg_free_blocks_count);
  printk("bg_free_inodes_count: %d\n", bg->bg_free_inodes_count);
  printk("bg_used_dirs_count: %d\n", bg->bg_used_dirs_count);
}

static void dump_ext2_inode(struct ext2_inode *i) {
  printk("ext2_inode dump: %p\n", i);
  printk("sizeof *i: %d\n", sizeof(*i));
  printk("i_mode: %p\n", i->i_mode);
  printk("i_size: %d\n", i->i_size);
  printk("i_blocks: %d\n", i->i_blocks);
  for(int b = 0; b < 15; b++)
    printk("i_block[%d]: %p\n", b, i->i_block[b]);
}

static void dump_dirent(struct dirent *d) {
  printk("dirent dump: %p\n", d);
  printk("inode: %d\n", d->inode);
  printk("rec_len: %d\n", d->rec_len);
  printk("name_len: %d\n", d->name_len);
  printk("file_type: %d\n", d->file_type);
  printk("name: %s\n", d->name);
}

static struct buf *ext2_get_sb() {
  return bio_read(1);
}

static struct buf *ext2_get_bg() {
  return bio_read(2);
}

/* for debug */
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

static struct buf *read_indirect_block(u32 *map, int bnum) {
  int idx = bnum - 12;
  return bio_read(map[idx]);
}

static int find_free_ino(struct buf *bitmap) {
  char chunk = 0xff;
  int inum = 1;
  for(int i = 0; i < sb.bsize; i++) {
    if((chunk = bitmap->data[i]) != 0xff)
      break;
    inum += 8;
  }

  if(chunk == 0xff)   /* no free inode */
    return -1;

  for(; chunk & 1; chunk >>= 1)
    inum++;

  return inum;
}

static int find_free_block(struct buf *bitmap) {
  char chunk = 0xff;
  int bnum = 0;
  for(int i = 0; i < sb.bsize; i++) {
    if((chunk = bitmap->data[i]) != 0xff)
      break;
    bnum += 8;
  }

  if(chunk == 0xff)   /* no free inode */
    return -1;

  for(; chunk & 1; chunk >>= 1)
    bnum++;

  return bnum;
}

static int ibmp_write_bit(struct buf *bitmap, int ino, int val) {
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
  c = bitmap->data[chunkn];
  if(val)
    c |= 1 << bitn;
  else
    c &= ~(char)(1 << bitn);
  bitmap->data[chunkn] = c;

  bio_write(bitmap);

  return 0;
}

static int bbmp_write_bit(struct buf *bitmap, int bn, int val) {
  int bitn = bn;
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
  c = bitmap->data[chunkn];
  if(val)
    c |= 1 << bitn;
  else
    c &= ~(char)(1 << bitn);
  bitmap->data[chunkn] = c;

  bio_write(bitmap);

  return 0;
}

static int ext2_alloc_block() {
  struct buf *bitmap = bio_read(sb.block_bitmap);
  int bn = find_free_block(bitmap);
  if(bn < 0)
    return -1;

  bbmp_write_bit(bitmap, bn, 1);

  bio_free(bitmap);

  return bn;
}

static struct ext2_inode *ext2_raw_inode(int inum, struct buf **b) {
  u32 bgrp = (inum - 1) / (sb.bsize / sb.inode_size);
  u64 offset = ((inum - 1) % (sb.bsize / sb.inode_size)) * sb.inode_size;
  struct buf *itable = bio_read(sb.inode_table + bgrp);
  if(b)
    *b = itable;

  return (struct ext2_inode *)(itable->data + offset);
}

struct buf *ext2_inode_block(struct inode *ino, int bi) {
  if(bi < 12) {
    return bio_read(ino->block[bi]);
  } else {
    struct buf *map = bio_read(ino->block[12]);
    struct buf *b = read_indirect_block((u32 *)map->data, bi);
    bio_free(map);
    return b;
  }

  return NULL;
}

static int ext2_grow_inode_block(struct inode *ino, int nblock) {
  int n;
  for(n = 0; n < 12; n++) {
    if(ino->block[n] == 0)
      goto found_free;
  }

  return -1;

found_free:
  for(int i = 0; i < nblock; i++) {
    int bn = ext2_alloc_block();
    ino->block[n + i] = bn;
  }

  ino->blocks += nblock * (sb.bsize / 512);

  inode_sync(ino);

  return 0;
}

static int ext2_append_inode_block(struct inode *ino, int blockn) {
  int i;
  for(i = 0; i < 12; i++) {
    if(ino->block[i] == 0)
      goto found_free;
  }

  /* unimpl: indirect block */
  return -1;

found_free:
  ino->block[i] = blockn;
  ino->blocks += sb.bsize / 512;

  inode_sync(ino);

  return 0;
}

static int ext2_alloc_inum() {
  struct buf *ibmp = bio_read(sb.inode_bitmap);

  int inum = find_free_ino(ibmp);
  if(inum < 0)
    return -1;

  ibmp_write_bit(ibmp, inum, 1);

  bio_free(ibmp);

  return inum;
}

static struct inode *ext2_alloc_inode() {
  int inum = ext2_alloc_inum();
  if(inum < 0)
    return NULL;

  return find_inode(inum);
}

#define roundup_4byte(m)  (((u64)(m)+3) & ~3)

/* @buf must be (sb.bsize)byte buffer */
static void make_dirent(u32 inode, char *name, u8 file_type, struct dirent *buf) {
  memset(buf, 0, sb.bsize);

  buf->inode = inode;
  buf->file_type = file_type;

  u32 name_len = strlen(name);
  buf->name_len = name_len;

  memcpy(buf->name, name, buf->name_len+1);

  u32 len = ((u64)buf->name + name_len) - (u64)buf;
  buf->rec_len = roundup_4byte(len);
}

static int ext2_dirlink(struct inode *pdir, struct dirent *de) {
  struct dirent *d;
  struct buf *b; 
  char *bend;
  char *dend;
  char *debegin;

  /* already exist? */
  if(ext2_search_dir(pdir, de->name) > 0)
    return -1;

  /* find free space */
  for(int i = 0; i < ext2_inode_nblock(pdir); i++) {
    b = ext2_inode_block(pdir, i);
    d = (struct dirent *)b->data;
    bend = b->data + sb.bsize;

    for(u64 bpos = 0; bpos < sb.bsize; bpos += d->rec_len) {
      d = (struct dirent *)(b->data + bpos);
      if(d->rec_len == 0) /* empty block */
        goto empty_block;
    }

    /* can append de */
    if((u64)bend - ((u64)d->name + strlen(d->name)) > de->rec_len)
      goto found;
  }

  if(ext2_inode_nblock(pdir) == 0) {
    int bn = ext2_alloc_block();
    ext2_append_inode_block(pdir, bn);
    b = bio_read(bn);
    d = (struct dirent *)b->data;
    goto empty_block;
  }

  /* no free space */
  return -1;

empty_block:
  /* d indicates b */
  d->inode = de->inode;
  d->rec_len = sb.bsize;
  d->name_len = de->name_len;
  d->file_type = de->file_type;
  memcpy(d->name, de->name, de->name_len + 1);

  bio_write(b);
  bio_free(b);

  return 0;

found:
  /* d indicates last dirent of block(b) */
  /* dirent must be aligned 4byte */
  dend = d->name + strlen(d->name);
  debegin = (char *)roundup_4byte(dend);
  d->rec_len = (u64)debegin - (u64)d;

  struct dirent *newd = (struct dirent *)debegin;
  newd->inode = de->inode;
  newd->rec_len = (u64)(bend - debegin);
  newd->name_len = de->name_len;
  newd->file_type = de->file_type;
  memcpy(newd->name, de->name, de->name_len + 1);

  bio_write(b);
  bio_free(b);

  return 0;
}

/* synchronize @i with disk inode */
static void inode_sync(struct inode *i) {
  struct buf *b;
  struct ext2_inode *e = ext2_raw_inode(i->inum, &b);
  
  e->i_mode = i->mode;
  e->i_size = i->size;
  e->i_atime = i->atime;
  e->i_ctime = i->ctime;
  e->i_mtime = i->mtime;
  e->i_dtime = i->dtime;
  e->i_links_count = i->links_count;
  e->i_blocks = i->blocks;
  memcpy(e->i_block, i->block, sizeof(u32) * 15);

  bio_write(b);
  bio_free(b);
}

static u8 itype_dtype_table[15] = {
  [1] = DT_FIFO,
  [2] = DT_CHR,
  [4] = DT_DIR,
  [6] = DT_BLK,
  [8] = DT_REG,
  [10] = DT_LNK,
  [12] = DT_SOCK,
};

static u8 imode2dtype(int mode) {
  int ftype = (mode & S_IFMT) >> 12;

  return itype_dtype_table[ftype];
}

static struct inode *ext2_new_inode(char *name, struct inode *dir, int mode, int dev) {
  char buf[1024];
  struct inode *ino = ext2_alloc_inode();
  if(!ino)
    return NULL;

  ino->mode = mode;
  ino->major = dev;
  ino->links_count = 1;

  if(S_ISDIR(mode)) {
    dir->links_count++;
    make_dirent(ino->inum, ".", DT_DIR, (struct dirent *)buf);
    if(ext2_dirlink(ino, (struct dirent *)buf) < 0)   /* dirlink "." */
      return NULL;
    make_dirent(dir->inum, "..", DT_DIR, (struct dirent *)buf);
    if(ext2_dirlink(ino, (struct dirent *)buf) < 0)   /* dirlink ".." */
      return NULL;

    ino->size = sb.bsize;
  } else if(S_ISCHR(mode)) {
    kinfo("new chardev\n");
    ino->size = 0;
  }

  make_dirent(ino->inum, name, imode2dtype(ino->mode), (struct dirent *)buf);
  if(ext2_dirlink(dir, (struct dirent *)buf) < 0)   /* dirlink new inode to dir */
    return NULL;

  inode_sync(ino);
  inode_sync(dir);

  return ino;
}

static int ext2_rm_inode(char *name, struct inode *pdir) {
  return -1;
}

struct inode *ext2_get_inode(int inum) {
  struct inode *i = find_inode(inum);
  struct buf *b;
  struct ext2_inode *e = ext2_raw_inode(inum, &b);

  if (!i)
    return NULL;

  i->mode = e->i_mode;
  i->size = e->i_size;
  i->atime = e->i_atime;
  i->ctime = e->i_ctime;
  i->mtime = e->i_mtime;
  i->dtime = e->i_dtime;
  i->links_count = e->i_links_count;
  i->blocks = e->i_blocks;
  memcpy(i->block, e->i_block, sizeof(u32) * 15);

  bio_free(b);

  return i;
}

/* for debug */
__attribute__((unused))
void ls_inode(struct inode *ino) {
  printk("ls inode %d nblock: %d\n", ino->inum, ext2_inode_nblock(ino));
  if(ino == NULL) {
    printk("null inode\n");
    return;
  }
  if(!S_ISDIR(ino->mode)) {
    printk("invalid inode: directory\n");
    return;
  }

  for(int i = 0; i < ext2_inode_nblock(ino); i++) {
    struct buf *dent = ext2_inode_block(ino, i);
    dump_dirent_block(dent->data);
    bio_free(dent);
  }
}

int ext2_read_inode(struct inode *ino, char *buf, u64 off, u64 size) {
  u32 bsize = sb.bsize;
  char *base = buf;

  if(off > ino->size)
    return -1;
  if(off + size > ino->size)
    size = ino->size - off;

  u32 offblk = off / bsize;
  u32 lastblk = (size + off) / bsize;
  u32 offblkoff = off % bsize;
  for(int i = offblk; i < ext2_inode_nblock(ino) && i <= lastblk; i++) {
    struct buf *b = ext2_inode_block(ino, i);
    u64 cpsize = min(size, bsize);
    if(offblkoff + cpsize > bsize)
      cpsize = bsize - offblkoff;

    memcpy(buf, b->data + offblkoff, cpsize);

    buf += cpsize;
    size -= cpsize;
    offblkoff = 0;

    bio_free(b);
  }

  return buf - base;
}

int ext2_w_inode(struct inode *ino, char *buf, u64 off, u64 size) {
  u32 bsize = sb.bsize;
  u64 sz = size;

  if(off > ino->size)
    return -1;
  if(off + size > ino->size)
    ino->size = off + size;

  u32 offblk = off / bsize;
  u32 lastblk = (size + off) / bsize;
  u32 offblkoff = off % bsize;

  if(lastblk > ext2_inode_nblock(ino))
    ext2_grow_inode_block(ino, lastblk - ext2_inode_nblock(ino));

  for(int i = offblk; i < ext2_inode_nblock(ino) && i <= lastblk; i++) {
    struct buf *b = ext2_inode_block(ino, i);
    u64 cpsize = min(size, bsize);
    if(offblkoff + cpsize > bsize)
      cpsize = bsize - offblkoff;

    memcpy(b->data + offblkoff, buf, cpsize);
    bio_write(b);

    buf += cpsize;
    size = size > bsize? size - bsize : 0;
    offblkoff = 0;

    bio_free(b);
  }

  inode_sync(ino);

  return sz;
}

int ext2_rm(char *path) {
  char namebuf[DIRENT_NAME_MAX] = {0};
  struct inode *pdir = ext2_path2inode_parent(path, namebuf);
  if(!pdir)
    return -1;

  return ext2_rm_inode(namebuf, pdir);
}

struct inode *ext2_mknod(char *path, int mode, int dev) {
  char namebuf[DIRENT_NAME_MAX] = {0};
  struct inode *pdir = ext2_path2inode_parent(path, namebuf);
  if(!pdir)
    return NULL;

  return ext2_new_inode(namebuf, pdir, mode, dev);
}

struct inode *ext2_mkdir(char *path) {
  char namebuf[DIRENT_NAME_MAX] = {0};
  struct inode *pdir = ext2_path2inode_parent(path, namebuf);
  if(!pdir)
    return NULL;

  return ext2_new_inode(namebuf, pdir, S_IFDIR, 0);
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

static int search_dirent_block(unsigned char *blk, char *path) {
  struct dirent *d;
  char buf[DIRENT_NAME_MAX];

  for(u64 bpos = 0; bpos < sb.bsize; bpos += d->rec_len) {
    d = (struct dirent *)(blk + bpos);

    memset(buf, 0, DIRENT_NAME_MAX);
    memcpy(buf, d->name, d->name_len);
    if(strcmp(buf, path) == 0)
      return d->inode;
  }

  return -1;
}

static int ext2_search_dir(struct inode *dir, char *name) {
  int inum;
  for(int i = 0; i < ext2_inode_nblock(dir); i++) {
    struct buf *db = ext2_inode_block(dir, i);
    if((inum = search_dirent_block(db->data, name)) > 0) {
      bio_free(db);
      return inum;
    }
    bio_free(db);
  }

  return -1;
}

/* traverse and return inode by path */
static struct inode *traverse_inode(struct inode *pi, char *path, char *name) {
  int err = 0;
  path = skippath(path, name, &err);
  if(err)
    return NULL;
  if(*path == 0 && *name == 0)
    return pi;
  if(!S_ISDIR(pi->mode))
    return pi;

  int inum = ext2_search_dir(pi, name);
  if(inum < 0)
    return NULL;

  memset(name, 0, DIRENT_NAME_MAX);

  return traverse_inode(ext2_get_inode(inum), path, name);
}

static struct inode *traverse_inode_parent(struct inode *pi, char *path, char *name) {
  int err = 0;
  path = skippath(path, name, &err);
  if(err)
    return NULL;
  if(*path == 0)
    return pi;
  if(!S_ISDIR(pi->mode))
    return pi;

  int inum = ext2_search_dir(pi, name);
  if(inum < 0)
    return NULL;

  memset(name, 0, DIRENT_NAME_MAX);

  return traverse_inode_parent(ext2_get_inode(inum), path, name);
}

/* return inode by path */
struct inode *ext2_path2inode(char *path) {
  struct inode *ino;

  if(*path == '/')
    ino = ext2_get_inode(EXT2_ROOT_INO);
  else
    ino = myproc()->cwd;

  char name[DIRENT_NAME_MAX] = {0};
  ino = traverse_inode(ino, path, name);

  return ino;
}

/* return parent inode of inode by path */
struct inode *ext2_path2inode_parent(char *path, char *namebuf) {
  struct inode *ino;

  if(*path == '/')
    ino = ext2_get_inode(EXT2_ROOT_INO);
  else
    ino = myproc()->cwd;

  ino = traverse_inode_parent(ino, path, namebuf);

  return ino;
}

void ext2_init() {
  struct buf *b_esb = ext2_get_sb();
  struct ext2_superblock *esb = (struct ext2_superblock *)b_esb->data;

  if(esb->s_magic != 0xef53)
    panic("invalid filesystem");

  struct buf *b_bg = ext2_get_bg();
  struct ext2_bg_desc *bg = (struct ext2_bg_desc *)b_bg->data;

  sb.bsize = 1024 << esb->s_log_block_size;
  sb.block_bitmap = bg->bg_block_bitmap;
  sb.inode_bitmap = bg->bg_inode_bitmap;
  sb.inode_table = bg->bg_inode_table;

  sb.inodes_count = esb->s_inodes_count;
  sb.blocks_count = esb->s_blocks_count;
  sb.first_data_block = esb->s_first_data_block;
  sb.blocks_per_group = esb->s_blocks_per_group;
  sb.inodes_per_group = esb->s_inodes_per_group;
  sb.mtime = esb->s_mtime;
  sb.wtime = esb->s_wtime;
  sb.first_ino = esb->s_first_ino;
  sb.inode_size = esb->s_inode_size;

  bio_free(b_esb);
  bio_free(b_bg);
}
