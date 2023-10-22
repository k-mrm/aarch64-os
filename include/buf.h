#ifndef AARCH64_OS_BUF_H
#define AARCH64_OS_BUF_H

#include "kernel.h"

#define NBUF  32

struct buf {
  unsigned char data[1024];
  u32 bno;
  u32 ref;
  u8 dirty;
  u8 valid;
};

void buf_init(void);
struct buf *bio_read(u32 bno);
void bio_write(struct buf *b);
void bio_free(struct buf *b);

#endif
