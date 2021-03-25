#include "systimer.h"
#include "printk.h"

void systimer_init(u32 interval) {
  printk("systimer init\n");
  u32 t = REG(SYSTIMER_CLO) + interval * 1000;
  REG(SYSTIMER_C(0)) = t;
}

void systimer_handle_irq(void) {
  u32 t = REG(SYSTIMER_CLO) + 1000 * 1000;

  REG(SYSTIMER_C(0)) = t;
  REG(SYSTIMER_CS) = 1;
  printk("systimer irq\n");
}

u64 systime() {
  u32 lo = REG(SYSTIMER_CLO);
  u32 hi = REG(SYSTIMER_CHI);

  return ((u64)hi << 32) | lo;
}

void sleep(u32 sec) {
  u64 s = systime() + sec * 1000 * 1000;

  while(systime() < s) {}
}
