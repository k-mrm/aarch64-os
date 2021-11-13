#ifndef AARCH64_OS_BUF_H
#define AARCH64_OS_BUF_H

struct buf {
  char data[1024];
  u32 bno;
  int free;
  int dirty;
};

struct buf bcache[NBUF];

#endif
