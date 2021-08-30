#ifndef MONO_DRIVER_GICV2_H
#define MONO_DRIVER_GICV2_H

#include "driver/virt/gicv2.h"

#define PGICV2_BASE  VIRT_GICV2_BASE

void gicv2_init(void);
void gic_eoi(u32 iar);
u32 gic_iar(void);

bool gic_enabled(void);

#endif
