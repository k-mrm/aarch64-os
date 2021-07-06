#ifndef MONO_TRAP_H
#define MONO_TRAP_H

#include "mono.h"

#define TIMER_IRQ   27

struct trapframe {
  u64 x0;
  u64 x1;
  u64 x2;
  u64 x3;
  u64 x4;
  u64 x5;
  u64 x6;
  u64 x7;
  u64 x8;
  u64 x9;
  u64 x10;
  u64 x11;
  u64 x12;
  u64 x13;
  u64 x14;
  u64 x15;
  u64 x16;
  u64 x17;
  u64 x18;
  u64 x19;
  u64 x20;
  u64 x21;
  u64 x22;
  u64 x23;
  u64 x24;
  u64 x25;
  u64 x26;
  u64 x27;
  u64 x28;
  u64 x29;
  u64 x30;  /* lr */
};

typedef void (*handler_t)(void);

void sync_handler(struct trapframe *tf);
void new_irq(int intid, handler_t handler);

void trap_init(void);

#endif
