#ifndef AARCH64_OS_BUF_H
#define AARCH64_OS_BUF_H

#include "kernel.h"

#define NBUF  32

struct buf {
  char data[1024];
  u32 bno;
  u8 free;
  u8 dirty;
  u8 valid;
};

struct buf bcache[NBUF];

void buf_init(void);
struct buf *bio_read(u32 bno);
void bio_write(struct buf *buf);

#endif
