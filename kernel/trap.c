#include "trap.h"
#include "aarch64.h"
#include "kernel.h"
#include "memmap.h"
#include "printk.h"
#include "proc.h"
#include "syscall.h"
#include "driver/gicv3.h"
#include "log.h"
#include "mm.h"

void syscall(struct trapframe *tf);
void dump_tf(struct trapframe *tf);

#define NIRQ  1024

void (*irqhandler[NIRQ])(void);

static void default_handler() {
  panic("unknown irq");
}

void trap_init() {
  for(int i = 0; i < NIRQ; i++)
    irqhandler[i] = default_handler;
}

void new_irq(int intid, void (*handler)(void)) {
  kinfo("new irq: %d\n", intid);
  if(intid < NIRQ)
    irqhandler[intid] = handler;
}

/* from EL0 */
extern void exit(int);
void fault_die(struct proc *p, char *why) {
  printk("%s\n", why);
  killproc(p, 1);
}

void handle_data_abort(struct trapframe *tf, int user, u64 esr) {
  struct proc *p = myproc();
  u64 dfsc = esr & 0x3f;

  if(p && p->fault_handler) {
    tf->elr = (u64)p->fault_handler;
    tf->x0 = (u64)p;
    return;
  }

  /* TODO */
  (void)dfsc;

  printk("elr %p far %p\n", elr_el1(), far_el1());
  if(user) {
    fault_die(p, "data abort EL0");
  } else {
    panic("data abort EL1");
  }
}

void handle_inst_abort(struct trapframe *tf, int user, u64 esr) {
  if(user)
    printk("user ");
  printk("instruction abort: elr: %p far: %p\n", elr_el1(), far_el1());
  dump_tf(tf);
  panic("instruction abort");
}

void ksync_handler(struct trapframe *tf) {
  u64 esr = esr_el1();
  u64 ec = (esr >> 26) & 0x3f;

  switch(ec) {
    case 0b100101:
      handle_data_abort(tf, 0, esr);
      return;
    case 0b000111:
      return;
    case 0b100110:
      panic("sp alignment fault");
    case 0b100001:
      handle_inst_abort(tf, 0, esr);
      return;
    default:
      printk("elr: %p far: %p\n", elr_el1(), far_el1());
      dump_tf(tf);
      panic("unknown ec: %d", ec);
  }
}

void usync_handler(struct trapframe *tf) {
  u64 esr = esr_el1();
  u64 ec = (esr >> 26) & 0x3f;

  switch(ec) {
    case 0b010101:  /* svc */
      syscall(tf);
      break;
    case 0b000111:
      break;
    case 0b100110:
      panic("sp alignment fault");
    case 0b100100:
      handle_data_abort(tf, 1, esr);
      break;
    case 0b100000:
      handle_inst_abort(tf, 1, esr);
      break;
    default:
      printk("elr: %p far: %p\n", elr_el1(), far_el1());
      dump_tf(tf);
      panic("unknown ec: %d", ec);
  }

  sigcheck(myproc());
}

void kirq_handler(struct trapframe *tf) {
  u32 iar = gic_iar();
  u32 intid = iar & 0x3ff;

  if(intid == 1023) {
    printk("warn: spurious interrupt\n");
    return;
  }

  if(irqhandler[intid])
    irqhandler[intid]();

  gic_eoi(iar);

  /* disable preemption from kernel mode */
  /*
  if(myproc() && myproc()->state == RUNNING && intid == TIMER_IRQ)
    yield();
    */
}

void uirq_handler(struct trapframe *tf) {
  struct proc *p = myproc();
  if(!p)
    panic("?");

  u32 iar = gic_iar();
  u32 intid = iar & 0x3ff;

  if(intid == 1023) {
    printk("warn: spurious interrupt\n");
    return;
  }

  if(irqhandler[intid])
    irqhandler[intid]();

  gic_eoi(iar);

  if(!p->sig && intid == TIMER_IRQ)
    yield();

  sigcheck(p);
}

void unknownint(int arg) {
  panic("unknown interrupt: elr: %p arg: %d", elr_el1(), arg);
}

void debug_trap(struct trapframe *tf) {
  dump_tf(tf);
  panic("debug trap");
}

void dump_tf(struct trapframe *tf) {
  printk("trapframe dump: %p\n", tf);
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
