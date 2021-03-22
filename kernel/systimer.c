#include "systimer.h"

void systimer_init(u32 interval) {
  u64 t = REG(SYSTIMER_CLO) + interval * 1000;
  REG(SYSTIMER_C1) = t;
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
