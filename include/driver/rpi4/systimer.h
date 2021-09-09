#ifndef AARCH64_OS_SYSTIMER_H
#define AARCH64_OS_SYSTIMER_H

#include "mono.h"
#include "memmap.h"

#define SYSTIMER_BASE (PERIPHERAL_BASE + 0x3000)

#define SYSTIMER_CS (SYSTIMER_BASE + 0x0)
#define SYSTIMER_CLO (SYSTIMER_BASE + 0x4)
#define SYSTIMER_CHI (SYSTIMER_BASE + 0x8)
#define SYSTIMER_C(n) (SYSTIMER_BASE + 0xc + (n) * 4)

void systimer1_init(u32 interval);
void systimer1_handle_irq(void);
u64 systime(void);
void sleep(u32 sec);
void set_systimer_c1(u32 ms);

struct systimer {
  int id;
  u32 interval;
};

#endif
