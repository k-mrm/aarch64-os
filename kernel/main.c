#include "kernel.h"
#include "aarch64.h"
#include "console.h"
#include "printk.h"
#include "trap.h"
#include "proc.h"
#include "driver/gicv2.h"
#include "driver/timer.h"
#include "driver/virtio.h"
#include "log.h"
#include "kalloc.h"
#include "ramdisk.h"
#include "fs.h"
#include "file.h"
#include "buf.h"

int main(void) {
  trap_init();
  console_init();
  printk("booting...\n");
  kinfo("kernel main @%p\n", main);
  gicv2_init();
  timer_init(200);
  kalloc_init1();
  pgt_init();
  kalloc_init2();
  virtio_init();
  buf_init();
  fs_init();
  file_init();
  proc_init();

  kinfo("cpuid: %d\n", mpidr_el1() & 0xff);
  kinfo("current EL: %d\n", cur_el());
  kinfo("vbar_el1: %p\n", vbar_el1());
  kinfo("ttbr1_el1: %p\n", ttbr1_el1());
  kinfo("cntfrq_el0: %d\n", cntfrq_el0());

  enable_irq();

  userproc_init();

  schedule();
}
