#ifndef AARCH64_OS_MAILBOX_H
#define AARCH64_OS_MAILBOX_H

#include "kernel.h"
#include "memmap.h"

#define MBOXBASE  (PERIPHERAL_BASE + 0xb880)

#define MBOX0_READ  (MBOXBASE + 0)
#define MBOX0_PEEK  (MBOXBASE + 0x10)
#define MBOX0_SENDER  (MBOXBASE + 0x14)
#define MBOX0_STATUS  (MBOXBASE + 0x18)
#define MBOX0_CFG (MBOXBASE + 0x1c)
#define MBOX0_WRITE (MBOXBASE + 0x20)

#define MBOX_EMPTY  0x40000000
#define MBOX_FULL 0x80000000

#define MBOX_SUCCESS  0x80000000

enum mbox_proptag {
  PROP_END = 0,
  PROP_FB_ALLOC_BUFFER = 0x40001,
  PROP_FB_GET_PITCH = 0x40008,
  PROP_FB_SET_PHY_WH = 0x48003,
  PROP_FB_SET_VIRT_WH = 0x48004,
  PROP_FB_SET_DEPTH = 0x48005,
};

u32 mbox_read(int ch);
void mbox_write(int ch, u32 data);

#endif
