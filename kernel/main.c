#include "mono.h"
#include "aarch64.h"
#include "console.h"
#include "printk.h"
#include "trap.h"
#include "proc.h"
#include "driver/gicv2.h"
#include "driver/timer.h"
#include "log.h"
#include "kalloc.h"

/* test */
void proc1() {
  u64 s = 0;
  for(;;) {
    printk("proc1: %p %d\n", s++, getpid());
  }
}

void proc2() {
  u64 s = 0;
  for(;;) {
    printk("proc2: %p %d\n", s++, getpid());
  }
}

void proc3() {
  u64 s = 0;
  for(;;) {
    printk("proc3: %p %d\n", s++, getpid());
  }
}

char *main_kstack[4096]; // main use this stack

int main(void) {
  console_init();
  gicv2_init();
  trap_init();
  timer_init(200);
  kalloc_init();
  proc_init();

  kinfo("mono os for aarch64\n");
  kinfo("cpuid: %d\n", mpidr_el1() & 0xff);
  kinfo("current EL: %d\n", cur_el());
  kinfo("vbar_el1: %p\n", vbar_el1());
  kinfo("cntfrq_el0: %d\n", cntfrq_el0());

  enable_irq();
  
  newproc(proc1);
  newproc(proc2);

  schedule();
}
