#include "ulib.h"
#include "usys.h"
#include "stat.h"
#include "fcntl.h"
#include "ext2.h"

void ls(char *path) {
  int fd = open(path, O_RDONLY);
  char buf[1024];
  if(fd < 0) {
    puts("No such file or directory");
    return;
  }

  struct stat st;
  fstat(fd, &st);

  if(st.st_mode & S_IFREG) {
    puts(path);
  } else if(st.st_mode & S_IFDIR) {
    while(read(fd, buf, sizeof(buf)) == sizeof(buf)) {
      struct dirent *d = (struct dirent *)buf;
      char *bend = buf + sizeof(buf);
      char *cd;
      char namebuf[DIRENT_NAME_MAX];

      while(d != bend && d->inode != 0) {
        memset(namebuf, 0, DIRENT_NAME_MAX);
        memcpy(namebuf, d->name, d->name_len);
        puts(namebuf);

        cd = (char *)d;
        cd += d->rec_len;
        d = (struct dirent *)cd;
      }
    }
  }

  close(fd);
}

int main(int argc, char **argv) {
  if(argc == 1) {
    ls(".");
  } else {
    for(int i = 1; i < argc; i++) {
      ls(argv[i]);
    }
  }
  exit(0);
}
