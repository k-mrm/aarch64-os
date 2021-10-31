#include "trap.h"
#include "aarch64.h"
#include "kernel.h"
#include "memmap.h"
#include "printk.h"
#include "proc.h"
#include "syscall.h"
#include "driver/gicv2.h"
#include "log.h"

void syscall(struct trapframe *tf);
void dump_tf(struct trapframe *tf);

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
  exit(1);
}

void handle_data_abort(int el, u64 esr) {
  u64 dfsc = esr & 0x3f;

  /* TODO */

  printk("elr %p far %p\n", elr_el1(), far_el1());
  if(el == 0)
    fault_die("data abort EL0");
  else
    panic("data abort EL1");
}

void handle_inst_abort(int el, u64 esr) {
  printk("elr: %p far: %p\n", elr_el1(), far_el1());
  panic("instruction abort");
}

void sync_handler(struct trapframe *tf) {
  // kinfo("sync handler: elr %p\n", tf->elr);

  u64 esr = esr_el1();
  u64 ec = (esr >> 26) & 0x3f;
  switch(ec) {
    case 0b100101:
      handle_data_abort(1, esr);
      return;
    case 0b010101:  /* svc */
      syscall(tf);
      return;
    case 0b000111:
      return;
    case 0b100110:
      panic("sp alignment fault");
    case 0b100100:
      handle_data_abort(0, esr);
      return;
    case 0b100000:
      handle_inst_abort(0, esr);
      return;
    case 0b100001:
      handle_inst_abort(1, esr);
      return;
    default:
      printk("elr: %p far: %p\n", elr_el1(), far_el1());
      dump_tf(tf);
      panic("unknown ec: %d", ec);
  }
}

void kirq_handler(struct trapframe *tf) {
  kinfo("irq handler: elr %p EL%d\n", tf->elr, cur_el());

  u32 iar = gic_iar();
  u32 targetcpuid = iar >> 10;
  u32 intid = iar & 0x3ff;

  // printk("kint %d\n", intid);
  irqhandler[intid]();

  gic_eoi(iar);

  if(curproc && curproc->state == RUNNING && intid == TIMER_IRQ)
    yield();
}

void uirq_handler(struct trapframe *tf) {
  kinfo("irq handler: elr %p EL%d\n", tf->elr, cur_el());

  u32 iar = gic_iar();
  u32 targetcpuid = iar >> 10;
  u32 intid = iar & 0x3ff;

  // printk("uint %d\n", intid);
  irqhandler[intid]();

  gic_eoi(iar);

  if(curproc && curproc->state == RUNNING && intid == TIMER_IRQ)
    yield();
}

void unknownint(int arg) {
  printk("elr: %p arg: %d\n", elr_el1());
  panic("unknown interrupt: arg: %d", arg);
}

void dump_tf(struct trapframe *tf) {
  printk("trapframe dump:\n");
  printk("x0 %p x1 %p x2 %p x3 %p\n", tf->x0, tf->x1, tf->x2, tf->x3);
  printk("x4 %p x5 %p x6 %p x7 %p\n", tf->x4, tf->x5, tf->x6, tf->x7);
  printk("x8 %p x9 %p x10 %p x11 %p\n", tf->x8, tf->x9, tf->x10, tf->x11);
  printk("x12 %p x13 %p x14 %p x15 %p\n", tf->x12, tf->x13, tf->x14, tf->x15);
  printk("x16 %p x17 %p x18 %p x19 %p\n", tf->x16, tf->x17, tf->x18, tf->x19);
  printk("x16 %p x17 %p x18 %p x19 %p\n", tf->x16, tf->x17, tf->x18, tf->x19);
  printk("x20 %p x21 %p x22 %p x23 %p\n", tf->x20, tf->x21, tf->x22, tf->x23);
  printk("x24 %p x25 %p x26 %p x27 %p\n", tf->x24, tf->x25, tf->x26, tf->x27);
  printk("x28 %p x29 %p x30 %p elr %p\n", tf->x28, tf->x29, tf->x30, tf->elr);
  printk("spsr %p sp %p\n", tf->spsr, tf->sp);
}
