#include "kernel.h"
#include "aarch64.h"
#include "console.h"
#include "printk.h"
#include "trap.h"
#include "proc.h"
#include "driver/gicv2.h"
#include "driver/timer.h"
#include "log.h"
#include "kalloc.h"
#include "ext2.h"
#include "ramdisk.h"

extern u64 __usr_begin;
extern u64 __usr_end;
extern u64 init;

int main(void) {
  console_init();
  kinfo("booting...\n");
  kinfo("kernel main @%p\n", main);
  gicv2_init();
  trap_init();
  timer_init(200);
  kalloc_init1();
  pgt_init();
  kalloc_init2();
  proc_init();
  ramdisk_init();

  fs_init(diskread(0));

  kinfo("cpuid: %d\n", mpidr_el1() & 0xff);
  kinfo("current EL: %d\n", cur_el());
  kinfo("vbar_el1: %p\n", vbar_el1());
  kinfo("ttbr1_el1: %p\n", ttbr1_el1());
  kinfo("cntfrq_el0: %d\n", cntfrq_el0());

  enable_irq();

  u64 b = (u64)&__usr_begin;
  u64 e = (u64)&__usr_end;
  u64 s = (u64)&init;
  userproc_init(b, e - b, s - b);
  
  schedule();
}
