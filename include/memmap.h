#ifndef MONO_MEMMAP_H
#define MONO_MEMMAP_H

#include "driver/gicv2.h"
#include "driver/uart.h"

/*
 *  qemu does not support raspi4
 */
//#define PERIPHERAL_BASE 0xfe000000
//#define PERIPHERAL_BASE 0x3f000000

#define VA_START    0xffffff8000000000

#define V2P(v)  ((v) - VA_START)
#define P2V(p)  ((p) + VA_START)

#define KERNBASE    (0x40000000 + VA_START)
#define PHYMEM_END  (KERNBASE + 128 * 1024 * 1024)   /* 128MB */

#define GICV2_BASE  P2V(PGICV2_BASE)
#define UARTBASE    P2V(PUARTBASE)

#endif
