#include "mono.h"
#include "aarch64.h"
#include "gpio.h"
#include "console.h"
#include "printk.h"
#include "systimer.h"
#include "trap.h"
#include "gicv2.h"
#include "proc.h"

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
  console_init();
  // gicv2_init();
  trap_init();
  // systimer1_init(100);
  // proc_init();

  printk("mono os for aarch64\n");
  printk("cpuid: %d\n", mpidr_el1() & 0xff);
  printk("current EL: %d\n", cur_el());
  printk("vbar_el1: %p\n", vbar_el1());

  newproc(proc1);
  newproc(proc2);

  for(;;)
    schedule();

  panic("why");
}
