#include "mono.h"
#include "mailbox.h"

u32 mbox_read(int ch) {
  for(;;) {
    while(REG(MBOX0_STATUS) & MBOX_EMPTY) {}

    u32 r = REG(MBOX0_READ);

    if((r & 0xf) == ch) {
      return r;
    }
  }
}

void mbox_write(int ch, u32 data) {
  while(REG(MBOX0_STATUS) & MBOX_FULL) {}

  REG(MBOX0_WRITE) = (data & ~0xf) | (ch & 0xf);
}
