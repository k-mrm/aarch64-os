#ifndef UNI_MEMMAP_H
#define UNI_MEMMAP_H

#include "uni.h"

/* BCM2711 */

#define PERIPHERAL_BASE 0xfe000000
//#define PERIPHERAL_BASE 0x3f000000

#define REG(addr) (*(volatile u32 *)(addr))

#endif
