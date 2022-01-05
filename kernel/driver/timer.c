#include "kernel.h"
#include "aarch64.h"
#include "printk.h"
#include "trap.h"
#include "driver/timer.h"
#include "log.h"

/* aarch64 generic timer driver */

struct timer {
  u64 interval_ms;
} atimer;

#define CNTV_CTL_ENABLE (1 << 0)
#define CNTV_CTL_IMASK  (1 << 1)
#define CNTV_CTL_ISTATUS  (1 << 2)

static void enable_timer() {
  u64 c = cntv_ctl_el0();
  c |= CNTV_CTL_ENABLE;
  c &= ~CNTV_CTL_IMASK;
  set_cntv_ctl_el0(c);
}

static void disable_timer() {
  u64 c = cntv_ctl_el0();
  c &= ~CNTV_CTL_ENABLE;
  c |= CNTV_CTL_IMASK;
  set_cntv_ctl_el0(c);
}

bool timer_enabled() {
  u64 c = cntv_ctl_el0();
  return c & 1;
}

static void reload_timer(u64 interval_ms) {
  u64 interval_us = interval_ms * 1000;
  u64 interval_clk = interval_us * (cntfrq_el0() / 1000000);

  set_cntv_tval_el0(interval_clk);
}

void timerintr() {
  disable_timer();
  reload_timer(atimer.interval_ms);
  enable_timer();
}

void timer_init(u64 interval_ms) {
  atimer.interval_ms = interval_ms;

  new_irq(TIMER_IRQ, timerintr);
}

void timer_init_percpu() {
  disable_timer();
  reload_timer(atimer.interval_ms);
  enable_timer();
}
