#include "trap.h"
#include "arm.h"
#include "mono.h"
#include "memmap.h"
#include "printk.h"
#include "gicv2.h"
#include "systimer.h"


handler_t irqhandler[256];

static void default_handler() {
  panic("unknown irq");
}

void trap_init() {
  for(int i = 0; i < 256; i++) {
    irqhandler[i] = default_handler;
  }
}

void new_irq(int intid, handler_t handler) {
  irqhandler[intid] = handler;
}

void sync_handler(struct trapframe *tf) {
  printk("elr: %p far: %p\n", elr_el1(), far_el1());

  u64 esr = esr_el1();
  esr >>= 26;
  switch(esr & 0x3f) {
    case 0b100101:
      panic("data abort");
    case 0b010101:
      printk("software interrupt\n");
      break;
    case 0b100110:
      panic("sp alignment fault");
    default:
      printk("%d ", esr & 0x3f);
      panic("unknown");
  }
}

void irq_handler(struct trapframe *tf) {
  u32 iar = gic_iar();
  u32 targetcpuid = iar >> 10;
  u32 intid = iar & 0x3ff;

  printk("irq: %d cpu: %d\n", intid, targetcpuid);

  irqhandler[intid]();

  gic_eoi(iar);
}
