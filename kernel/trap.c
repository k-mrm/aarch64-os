#include "trap.h"
#include "aarch64.h"
#include "kernel.h"
#include "memmap.h"
#include "printk.h"
#include "proc.h"
#include "syscall.h"
#include "driver/gicv2.h"

void syscall(struct trapframe *tf);

#define NIRQ  256

handler_t irqhandler[NIRQ];

static void default_handler() {
  panic("unknown irq");
}

void trap_init() {
  for(int i = 0; i < NIRQ; i++) {
    irqhandler[i] = default_handler;
  }
}

void new_irq(int intid, handler_t handler) {
  irqhandler[intid] = handler;
}

/* from EL0 */
void fault_die(char *reason) {
  printk("%s\n", reason);
  _exit(1);
}

void handle_data_abort(int el, u64 esr) {
  u64 dfsc = esr & 0x3f;

  /* TODO */

  printk("far %p\n", far_el1());
  if(el == 0) {
    fault_die("data abort EL0");
  }
  else {
    panic("data abort EL1");
  }
}

void handle_inst_abort(int el, u64 esr) {
  panic("instruction abort");
}

void sync_handler(struct trapframe *tf) {
  printk("sync handler: elr %p\n", tf->elr);

  u64 esr = esr_el1();
  u64 ec = (esr >> 26) & 0x3f;
  switch(ec) {
    case 0b100101:
      handle_data_abort(1, esr);
      return;
    case 0b010101:  /* svc */
      syscall(tf);
      return;
    case 0b100110:
      panic("sp alignment fault");
    case 0b100100:
      handle_data_abort(0, esr);
      return;
    case 0b100000:
      handle_inst_abort(0, esr);
      return;
    default:
      printk("elr: %p far: %p\n", elr_el1(), far_el1());
      printk("%d ", esr & 0x3f);
      panic("unknown");
  }
}

void kirq_handler(struct trapframe *tf) {
  // printk("irq handler: elr %p EL%d\n", tf->elr, cur_el());

  u32 iar = gic_iar();
  u32 targetcpuid = iar >> 10;
  u32 intid = iar & 0x3ff;

  irqhandler[intid]();

  gic_eoi(iar);

  if(curproc && curproc->state == RUNNING && intid == TIMER_IRQ)
    yield();
}

void uirq_handler(struct trapframe *tf) {
  // printk("irq handler: elr %p EL%d\n", tf->elr, cur_el());

  u32 iar = gic_iar();
  u32 targetcpuid = iar >> 10;
  u32 intid = iar & 0x3ff;

  irqhandler[intid]();

  gic_eoi(iar);

  if(curproc && curproc->state == RUNNING && intid == TIMER_IRQ)
    yield();
}

void unknownint(int arg) {
  printk("elr: %p arg: %d\n", elr_el1(), arg);
  panic("unknown interrupt");
}
