#include "exception.h"
#include "arm.h"
#include "mono.h"
#include "memmap.h"
#include "printk.h"
#include "gicv2.h"
#include "systimer.h"

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
      printk("%d \n", esr & 0x3f);
      panic("unknown");
  }
}

void irq_handler(struct trapframe *tf) {
  u32 iar = REG(GICC_IAR);
  u32 intid = iar & 0x3ff;

  printk("irq: %d\n", intid);
  if(intid == 32 + 64) {
    systimer_handle_irq();
  }

  REG(GICC_EOIR) = iar;
}
