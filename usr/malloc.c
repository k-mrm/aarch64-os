#include "ulib.h"
#include "usys.h"

union memhdr {
  struct {
    union memhdr *ptr;
    u64 size;    /* unit */
  };
  long align;
};

static union memhdr base;
static union memhdr *freep = NULL;

#define MINALLOC 512

static union memhdr *morecore(u64 nunits) {
  if(MINALLOC > nunits) {
    nunits = MINALLOC;
  }

  void *p = sbrk(sizeof(union memhdr) * nunits);
  if(!p)
    return NULL;

  union memhdr *up = (union memhdr *)p;
  up->size = nunits;

  free((void *)(up + 1));

  return freep;
}

void *malloc(u64 nbytes) {
  if(nbytes == 0)
    return NULL;

  /* size of 1unit == sizeof(union memhdr) */
  u64 nunits = (nbytes + sizeof(union memhdr) - 1) / sizeof(union memhdr) + 1;

  union memhdr *prev = freep;

  if(!freep) { /* first call of malloc */
    prev = freep = &base;
    base.ptr = freep;
    base.size = 0;
  }

  for(union memhdr *p = prev->ptr; ; prev = p, p = prev->ptr) {
    if(p->size >= nunits) { /* enough */
      if(p->size == nunits) {
        prev->ptr = p->ptr;
      }
      else {
        p->size -= nunits;
        p += p->size;
        p->size = nunits;
      }
      freep = prev;
      return (void *)(p + 1);
    }

    if(p == freep) {
      if((p = morecore(nunits)) == NULL)
        return NULL;
    }
  }
}

void free(void *ptr) {
  if(!ptr)
    return;

  union memhdr *bp = (union memhdr *)ptr - 1;

  union memhdr *p;
  for(p = freep; !(p < bp && bp < p->ptr); p = p->ptr) {
    if(p->ptr <= p && (p < bp || bp < p->ptr))
      break;
  }

  if(bp + bp->size == p->ptr) {
    bp->size += p->ptr->size;
    bp->ptr = p->ptr->ptr;
  }
  else {
    bp->ptr = p->ptr;
  }

  if(p + p->size == bp) {
    p->size += bp->size;
    p->ptr = bp->ptr;
  }
  else {
    p->ptr = bp;
  }

  freep = p;
}
