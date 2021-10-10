#ifndef AARCH64_OS_STAT_H
#define AARCH64_OS_STAT_H

#include "kernel.h"
#include "ext2.h"

/* st_mode */
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

struct stat {
  int st_dev;
  u32 st_ino;
  u16 st_mode;
  u32 st_size;
  u32 st_nlink;
};

#endif
