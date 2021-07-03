#include "mono.h"
#include "aarch64.h"
#include "console.h"
#include "printk.h"
#include "trap.h"
#include "proc.h"
#include "driver/gicv2.h"
#include "driver/timer.h"

/* test */
void proc1() {
  u64 s = 0;
  for(;;) {
    printk("proc1: %p daif %d\n", s++, gic_enabled());
  }
}

void proc2() {
  u64 s = 0;
  for(;;) {
    printk("proc2: %p daif %d\n", s++, gic_enabled());
  }
}

int main(void) {
  enable_irq();

  console_init();
  gicv2_init();
  trap_init();
  timer_init(100);
  // proc_init();

  printk("mono os for aarch64\n");
  printk("cpuid: %d\n", mpidr_el1() & 0xff);
  printk("current EL: %d\n", cur_el());
  printk("vbar_el1: %p\n", vbar_el1());
  printk("cntfrq_el0: %d\n", cntfrq_el0());
  printk("daif: %p\n", daif());

  gic_set_pending(27);

  /*
  newproc(proc1);
  newproc(proc2);

  for(;;)
    schedule();
    */
  for(;;);

  panic("why");
}
