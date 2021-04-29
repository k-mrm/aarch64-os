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

/* test */
int fib(int n) {
  if(n < 2) {
    return n;
  }
  else {
    return fib(n - 1) + fib(n - 2);
  }
}

void proc1() {
  while(1) {
    printk("proc1 %d", fib(40));
  }
}

void proc2() {
  while(1) {
    printk("proc2 %d", fib(40));
  }
}

int main(void) {
  console_init();
  uart_init(0);
  gicv2_init();
  trap_init();
  systimer1_init(200);

  for(;;) {
    uart_putc(0, 'a');
    sleep(1);
  }
  uart_puts(0, "Hello, uart!\n");

  printk("mono os for raspberry pi 4\n");
  printk("cpuid: %d\n", mpidr_el1() & 0xff);
  printk("%d %u\n", -1, -1);
  printk("systime %d\n", systime());
  printk("current EL: %d\n", cur_el());
  printk("%s%c\n", "Hello, raspi4", '!');
  printk("addr: %p\n", printk);
  printk("var_el1: %p\n", vbar_el1());

  enable_irq();

  /*
  newproc(proc1);
  newproc(proc2);

  schedule();
  */

  for(;;) {}

  return 0;
}
