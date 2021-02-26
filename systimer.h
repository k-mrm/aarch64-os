#ifndef UNI_SYSTIMER_H
#define UNI_SYSTIMER_H

#include "uni.h"
#include "memmap.h"

#define SYSTIMER_BASE (PERIPHERAL_BASE + 0x3000)

#define SYSTIMER_CS (SYSTIMER_BASE + 0x0)
#define SYSTIMER_CLO (SYSTIMER_BASE + 0x4)
#define SYSTIMER_CHI (SYSTIMER_BASE + 0x8)
#define SYSTIMER_C0 (SYSTIMER_BASE + 0xc)
#define SYSTIMER_C1 (SYSTIMER_BASE + 0x10)
#define SYSTIMER_C2 (SYSTIMER_BASE + 0x14)
#define SYSTIMER_C3 (SYSTIMER_BASE + 0x18)

u64 systime(void);
void sleep(u32 sec);

#endif
