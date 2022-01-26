#ifndef AARCH64_OS_DRIVER_GICV2_H
#define AARCH64_OS_DRIVER_GICV2_H

#ifndef __ASSEMBLER__

void gicv3_init(void);
void gicv3_init_percpu(void);

void gic_eoi(u32 iar);
u32 gic_iar(void);

bool gic_enabled(void);

#endif

#endif
