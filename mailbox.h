#ifndef UNI_MAILBOX_H
#define UNI_MAILBOX_H

#include "uni.h"
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

u32 mbox_read(int ch);
void mbox_write(int ch, u32 data);

#endif
