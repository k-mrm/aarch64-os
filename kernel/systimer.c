#include "systimer.h"
#include "printk.h"
#include "trap.h"

struct systimer timer1, timer3;

void systimer1_init(u32 interval) {
  printk("systimer init\n");
  timer1.id = 1;
  timer1.interval = interval;
  u32 t = REG(SYSTIMER_CLO) + interval * 1000;
  REG(SYSTIMER_C(1)) = t;

  new_irq(97, systimer1_handle_irq);
}

void systimer1_handle_irq(void) {
  static int counter = 1;
  u32 t = REG(SYSTIMER_CLO) + timer1.interval * 1000;

  REG(SYSTIMER_C(1)) = t;
  REG(SYSTIMER_CS) = 1 << 1;
  printk("systimer1 irq %d\n", counter++);
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
