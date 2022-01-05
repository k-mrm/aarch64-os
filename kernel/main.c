#include "kernel.h"
#include "aarch64.h"
#include "console.h"
#include "printk.h"
#include "trap.h"
#include "proc.h"
#include "driver/gicv2.h"
#include "driver/timer.h"
#include "driver/virtio.h"
#include "driver/psci.h"
#include "log.h"
#include "kalloc.h"
#include "ramdisk.h"
#include "fs.h"
#include "file.h"
#include "buf.h"

void _start(void);

volatile int started = 0;
int ncpu_active = 1;

int main(void) {
  if(cpuid() == 0) {  /* primary */
    trap_init();
    console_init();
    printk_init();
    printk("booting...\n");
    gicv2_init();
    gicv2_init_percpu();
    timer_init(200);
    timer_init_percpu();
    kalloc_init1();
    kpgt_init();
    pgt_init();
    kalloc_init2();
    virtio_init();
    buf_init();
    inode_init();
    file_init();
    proc_init();
    userproc_init();

    isb();

    if(psci_cpu_on(1, V2P(_start)) < 0)
      printk("warn: cpu1 wakeup failed\n");
  } else {  /* secondary */
    pgt_init();
    timer_init_percpu();
    gicv2_init_percpu();
    ncpu_active++;
    printk("core%d started %d\n", cpuid(), ncpu_active);
  }

  kinfo("cpuid: %d\n", cpuid());
  kinfo("current EL: %d\n", cur_el());
  kinfo("vbar_el1: %p\n", vbar_el1());
  kinfo("ttbr1_el1: %p\n", ttbr1_el1());
  kinfo("cntfrq_el0: %d\n", cntfrq_el0());
  kinfo("timer enabled?: %d\n", timer_enabled());
  kinfo("gic?%d\n", gic_enabled());

  schedule();
}
