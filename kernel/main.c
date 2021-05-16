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
void sum(int n, int id) {
  int s = 0;
  for(int i = 0; i < n; i++) {
    s += i;
    printk("proc %d: %d\n", id, s);
  }
}

void proc1() {
  while(1) {
    sum(10, 1);
  }
}

void proc2() {
  while(1) {
    sum(10, 2);
  }
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
  printk("var_el1: %p\n", vbar_el1());

  enable_irq();

  newproc(proc1);
  newproc(proc2);

  schedule();
}
