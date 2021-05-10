#ifndef MONO_MEMMAP_H
#define MONO_MEMMAP_H

#include "mono.h"

/* BCM2711 */

/*
 *  qemu does not support raspi4
 */
#define PERIPHERAL_BASE 0xfe000000
//#define PERIPHERAL_BASE 0x3f000000

#define REG(addr) (*(volatile u32 *)(u64)(addr))

#endif
