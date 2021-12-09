#include "kernel.h"
#include "buf.h"
#include "string.h"
#include "driver/virtio.h"
#include "printk.h"
#include "log.h"

struct buf bcache[NBUF];

void buf_init() {
  for(int i = 0; i < NBUF; i++) {
    struct buf *b = &bcache[i];
    memset(b, 0, sizeof(*b));
  }
}

static struct buf *get_buf(u32 bno) {
  for(int i = 0; i < NBUF; i++) {
    if(bcache[i].bno == bno)
      return &bcache[i];
  }

  for(int i = 0; i < NBUF; i++) {
    if(bcache[i].ref == 0) {
      bcache[i].ref = 1;
      bcache[i].bno = bno;
      return &bcache[i];
    }
  }

  panic("buf");
}

struct buf *bio_read(u32 bno) {
  struct buf *b = get_buf(bno);
  if(!b)
    return NULL;

  if(!b->valid) {
    if(virtio_blk_op(bno, b->data, DREAD) < 0)
      panic("disk read failed");
    b->valid = 1;
  }

  return b;
}

void bio_write(struct buf *b) {
  virtio_blk_op(b->bno, b->data, DWRITE);
  b->valid = 1;
}

struct buf *bio_dup(struct buf *b) {
  b->ref++;
  return b;
}

void bio_free(struct buf *b) {
  if(b->ref == 0)
    panic("bio_free");

  b->ref--;

  if(b->ref == 0) {
    memset(b, 0, sizeof(*b));
  }
}
