#include "mailbox.h"

unsigned int mbox_read(int ch) {
  for(;;) {
    while((REG(MBOX0_STATUS) & MBOX_EMPTY) != 0) {}

    unsigned int r = REG(MBOX0_READ);

    if((r & 0xf) == ch) {
      return r >> 4;
    }
  }
}

void mbox_write(int ch, unsigned int data) {
  while((REG(MBOX0_STATUS) & MBOX_FULL) != 0) {}

  REG(MBOX0_WRITE) = (data << 4) | (ch & 0xf);
}
