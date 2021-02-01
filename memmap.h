#ifndef UNI_MEMMAP_H
#define UNI_MEMMAP_H

/* BCM2711 */

#define PERIPHERAL_BASE 0xfe000000

#define REG(addr) (*(volatile unsigned int *)(addr))

#endif
