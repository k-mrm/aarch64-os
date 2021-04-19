#include "mono.h"
#include "arm.h"
#include "gpio.h"
#include "framebuffer.h"
#include "console.h"
#include "printk.h"
#include "systimer.h"
#include "trap.h"
#include "gicv2.h"

void proc() {
  ;
}

int main(void) {
  console_init();
  gicv2_init();
  trap_init();
  systimer1_init(1000);

  printk("mono os for raspberry pi 4\n");
  printk("cpuid: %d\n", mpidr_el1() & 0xff);
  printk("%d %u\n", -1, -1);
  printk("systime %d\n", systime());
  printk("current EL: %d\n", cur_el());
  printk("%s%c\n", "Hello, raspi4", '!');
  printk("addr: %p\n", printk);
  printk("var_el1: %p\n", vbar_el1());

  enable_irq();

  schedule();

  for(;;) {}

  return 0;
}
