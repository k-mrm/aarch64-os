#ifndef MONO_GPIO_H
#define MONO_GPIO_H

#include "memmap.h"

#define GPIOBASE (PERIPHERAL_BASE + 0x200000)

#define GPFSEL(n) (GPIOBASE + 0x4 * (n))
#define GPSET(n)  (GPIOBASE + 0x1c + 0x4 * (n))
#define GPCLR(n)  (GPIOBASE + 0x28 + 0x4 * (n))

void gpio_set(int pin);
void gpio_clr(int pin);

enum pinmode {
  INPUT = 0b000,
  OUTPUT = 0b001,
  ALT0 = 0b100,
  ALT1 = 0b101,
  ALT2 = 0b110,
  ALT3 = 0b111,
  ALT4 = 0b011,
  ALT5 = 0b010,
};

void set_pinmode(int pin, enum pinmode mode);

#endif
