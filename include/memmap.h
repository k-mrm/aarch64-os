#ifndef MONO_MEMMAP_H
#define MONO_MEMMAP_H

#include "mono.h"

/* BCM2711 */

/*
 *  qemu does not support raspi4
 */
//#define PERIPHERAL_BASE 0xfe000000
//#define PERIPHERAL_BASE 0x3f000000

#define KERNBASE    0x40000000
#define PHYMEM_END  (KERNBASE + 128 * 1024 * 1024)   /* 128MB */

#define VA_START    0xffffff8000000000

#endif
