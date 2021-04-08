#ifndef MONO_GPIO_H
#define MONO_GPIO_H

#include "memmap.h"

#define GPIOBASE (PERIPHERAL_BASE + 0x200000)

#define GPFSEL0 (GPIOBASE + 0x0)
#define GPFSEL1 (GPIOBASE + 0x4)
#define GPFSEL2 (GPIOBASE + 0x8)
#define GPFSEL3 (GPIOBASE + 0xc)
#define GPFSEL4 (GPIOBASE + 0x10)
#define GPFSEL5 (GPIOBASE + 0x14)

#define GPSET0  (GPIOBASE + 0x1c)
#define GPSET1  (GPIOBASE + 0x20)

#define GPCLR0  (GPIOBASE + 0x28)
#define GPCLR1  (GPIOBASE + 0x2c)

#endif
