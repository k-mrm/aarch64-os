#ifndef AARCH64_OS_MEMMAP_H
#define AARCH64_OS_MEMMAP_H

#include "driver/gicv2.h"
#include "driver/uart.h"

#define VA_START    0xffffff8000000000

#ifdef __ASSEMBLER__

#define V2P(v)  ((v) - VA_START)
#define P2V(p)  ((p) + VA_START)

#else

#define V2P(v)  ((u64)(v) - VA_START)
#define P2V(p)  ((u64)(p) + VA_START)

#endif

#define PKERNBASE   0x40000000
#define KERNBASE    (PKERNBASE + VA_START)
#define PHYMEMEND   (KERNBASE + 128 * 1024 * 1024)   /* 128MB */

#define USEREND     0x7fffffffff

#define USTACKTOP   0x7ffffff000
#define USTACKBOTTOM  (USTACKTOP - PAGESIZE)

#define PVIRTIO0  0xa000000

#define GICV2_BASE  P2V(PGICV2_BASE)
#define UARTBASE    P2V(PUARTBASE)
#define VIRTIO0     P2V(PVIRTIO0)

#endif
