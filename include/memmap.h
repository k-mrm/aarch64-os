#ifndef MONO_MEMMAP_H
#define MONO_MEMMAP_H

#include "driver/gicv2.h"
#include "driver/uart.h"

#define VA_START    0xffffff8000000000

#define V2P(v)  ((v) - VA_START)
#define P2V(p)  ((p) + VA_START)

#define PKERNBASE   0x40000000
#define KERNBASE    (PKERNBASE + VA_START)
#define PHYMEMEND   (KERNBASE + 128 * 1024 * 1024)   /* 128MB */

#define PKERNSECEND 0x40200000
#define KERNSECEND  (PKERNSECEND + VA_START)

#define GICV2_BASE  P2V(PGICV2_BASE)
#define UARTBASE    P2V(PUARTBASE)

#endif
