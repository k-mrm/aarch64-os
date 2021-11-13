#include "kernel.h"
#include "buf.h"
#include "string.h"
#include "driver/virtio.h"

void buf_init() {
  for(int i = 0; i < NBUF; i++) {
    struct buf *b = &bcache[i];
    memset(b, 0, 1024);
    b->bno = 0;
    b->dirty = 0;
    b->free = 1;
  }
}

static struct buf *alloc_buf() {
  for(int i = 0; i < NBUF; i++) {
    if(bcache[i].free) {
      bcache[i].free = 0;
      return &bcache[i];
    }
  }

  return NULL;
}

struct buf *bio_read(u32 bno) {
  struct buf *b = alloc_buf();

  virtio_blk_rw(bno, b->data, DREAD);
  b->bno = bno;

  return b;
}
