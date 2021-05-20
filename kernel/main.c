#include "mono.h"
#include "arm.h"
#include "gpio.h"
#include "framebuffer.h"
#include "console.h"
#include "printk.h"
#include "systimer.h"
#include "trap.h"
#include "uart.h"
#include "gicv2.h"
#include "proc.h"

/* test */
void task(int id) {
  u64 s = 0;
  for(;;) {
    printk("proc %d: %p daif %p\n", id, s++, daif());
  }
}

void proc1() {
  task(1);
}

void proc2() {
  task(2);
}

void proc3() {
  task(3);
}

int main(void) {
  console_init();
  gicv2_init();
  trap_init();
  systimer1_init(200);
  proc_init();

  printk("mono os for raspberry pi 4\n");
  printk("cpuid: %d\n", mpidr_el1() & 0xff);
  printk("current EL: %d\n", cur_el());
  printk("vbar_el1: %p\n", vbar_el1());

  newproc(proc2);
  newproc(proc1);
  newproc(proc3);

  schedule();
}
