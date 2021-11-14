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

static struct buf *get_buf(u32 bno) {
  for(int i = 0; i < NBUF; i++) {
    if(bcache[i].bno == bno)
      return &bcache[i];
  }

  for(int i = 0; i < NBUF; i++) {
    if(bcache[i].free) {
      bcache[i].free = 0;
      bcache[i].bno = bno;
      return &bcache[i];
    }
  }

  return NULL;
}

struct buf *bio_read(u32 bno) {
  struct buf *b = get_buf(bno);

  if(!b->valid) {
    virtio_blk_rw(bno, b->data, DREAD);
    b->valid = 1;
  }

  return b;
}

void bio_write(struct buf *b) {
  virtio_blk_rw(b->bno, b->data, DWRITE);
  b->valid = 1;
}
