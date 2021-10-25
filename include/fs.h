#ifndef AARCH64_OS_FS_H
#define AARCH64_OS_FS_H

#include "kernel.h"
#include "ext2.h"

/* inode.mode */
#define S_IFSOCK  EXT2_S_IFSOCK
#define S_IFLNK EXT2_S_IFLNK
#define S_IFREG EXT2_S_IFREG
#define S_IFBLK EXT2_S_IFBLK
#define S_IFDIR EXT2_S_IFDIR
#define S_IFCHR EXT2_S_IFCHR
#define S_IFIFO EXT2_S_IFIFO
                
#define S_ISUID EXT2_S_ISUID
#define S_ISGID EXT2_S_ISGID
#define S_ISVTX EXT2_S_ISVTX
                
#define S_IRUSR EXT2_S_IRUSR
#define S_IWUSR EXT2_S_IWUSR
#define S_IXUSR EXT2_S_IXUSR
#define S_IRGRP EXT2_S_IRGRP
#define S_IWGRP EXT2_S_IWGRP
#define S_IXGRP EXT2_S_IXGRP
#define S_IROTH EXT2_S_IROTH
#define S_IWOTH EXT2_S_IWOTH
#define S_IXOTH EXT2_S_IXOTH

#define NINODE  256

/* on memory inode */
struct inode {
  u16 mode;
  u16 uid;
  u32 size;
  u32 atime;
  u32 ctime;
  u32 mtime;
  u32 dtime;
  u16 gid;
  u16 links_count;
  u32 blocks;
  u32 flags;
  u32 block[15];

  u32 inum;
  u32 major;
  u32 minor;
};

void fs_init(char *img);
bool isdir(struct inode *ino);

#endif
