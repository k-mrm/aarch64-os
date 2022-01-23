#include "ulib.h"
#include "usys.h"
#include "stat.h"
#include "fcntl.h"
#include "dirent.h"

int ls(char *path) {
  char buf[1024];
  int fd = open(path, O_RDONLY);
  if(fd < 0) {
    puts("No such file or directory");
    return -1;
  }

  struct stat st;
  fstat(fd, &st);

  if(S_ISREG(st.st_mode) || S_ISCHR(st.st_mode)) {
    puts(path);
  } else if(S_ISDIR(st.st_mode)) {
    while(read(fd, buf, sizeof(buf)) == sizeof(buf)) {
      char namebuf[DIRENT_NAME_MAX];

      for(u64 bpos = 0; bpos < st.st_blksize; ) {
        struct dirent *d = (struct dirent *)(buf + bpos);
        if(d->rec_len == 0)
          break;
        memset(namebuf, 0, DIRENT_NAME_MAX);
        memcpy(namebuf, d->name, d->name_len);
        printf("%s\t%d\n", namebuf, d->inode);
        bpos += d->rec_len;
      }
    }
  }

  close(fd);
  return 0;
}

int main(int argc, char **argv) {
  int err = 0;

  if(argc == 1) {
    ls(".");
  } else {
    for(int i = 1; i < argc; i++) {
      if(ls(argv[i]) < 0)
        err = 1;
    }
  }

  exit(err);
}
