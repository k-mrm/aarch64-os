#ifndef AARCH64_OS_FS_H
#define AARCH64_OS_FS_H

#include "kernel.h"

/* inode.mode */

#define S_IFMT    0xf000

#define S_IFSOCK  0xc000
#define S_IFLNK   0xa000
#define S_IFREG   0x8000
#define S_IFBLK   0x6000
#define S_IFDIR   0x4000
#define S_IFCHR   0x2000
#define S_IFIFO   0x1000

#define S_ISUID   0x0800
#define S_ISGID   0x0400
#define S_ISVTX   0x0200

#define S_IRUSR   0x0100
#define S_IWUSR   0x0080
#define S_IXUSR   0x0040
#define S_IRGRP   0x0020
#define S_IWGRP   0x0010
#define S_IXGRP   0x0008
#define S_IROTH   0x0004
#define S_IWOTH   0x0002
#define S_IXOTH   0x0001

#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)

#define NINODE  256

struct superblock {
  u32 inodes_count;
  u32 blocks_count;
  u32 first_data_block;
  u32 blocks_per_group;
  u32 inodes_per_group;
  u32 mtime;
  u32 wtime;
  u32 first_ino;
  u16 inode_size;

  u32 bsize;
  char *block_bitmap;
  char *inode_bitmap;
  char *inode_table;
};

extern struct superblock sb;

/* on memory inode */
struct inode {
  u16 mode;
  u32 size;
  u32 atime;
  u32 ctime;
  u32 mtime;
  u32 dtime;
  u16 links_count;
  u32 blocks;
  u32 block[15];

  u32 inum;
  u32 major;
  u32 minor;
};

void fs_init(void);
struct inode *find_inode(int inum);
struct inode *path2inode(char *path);
struct inode *alloc_inode(void);
int read_inode(struct inode *ino, char *buf, u64 off, u64 size);

#endif
