#include "exception.h"
#include "arm.h"
#include "mono.h"
#include "printk.h"

void sync_handler(struct trapframe *tf) {
  u64 esr = esr_el1();
  esr >>= 26;
  switch(esr & 0x3f) {
    case 0b100101:
      panic("data abort");
      break;
    case 0b010101:
      printk("software interrupt\n");
      break;
    default:
      panic("unknown");
      break;
  }
  printk("%p\n", tf->x30);
}

void irq_handler(struct trapframe *tf) {
  printk("irq\n");
}
