#include "mono.h"

/* aarch64 generic timer driver */

#define CNTV_CTL_ENABLE (1 << 0)
#define CNTV_CTL_IMASK  (1 << 1)
#define CNTV_CTL_ISTATUS  (1 << 2)

void enable_timer() {
  u64 c = cntv_ctl_el0();
  c |= CNTV_CTL_ENABLE;
  c &= ~CNTV_CTL_IMASK;
  set_cntv_ctl_el0(c);
}

void disable_timer() {
  u64 c = cntv_ctl_el0();
  c &= ~CNTV_CTL_ENABLE;
  c |= CNTV_CTL_IMASK;
  set_cntv_ctl_el0(c);
}

void timer_init(u64 interval_ms) {
  enable_timer();
}
