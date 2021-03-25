#ifndef MONO_ARM_H
#define MONO_ARM_H

#include "mono.h"

static inline int cur_el() {
  int el;
  asm volatile("mrs %0, CurrentEL" : "=r"(el));

  return (el >> 2) & 3;
}

static inline u64 *vbar_el1() {
  u64 *v;
  asm volatile("mrs %0, vbar_el1" : "=r"(v));

  return v;
}

static inline void set_vbar_el1(u64 *v) {
  asm volatile("msr vbar_el1, %0" : : "r"(v));
}

static inline void wfe() {
  asm volatile("wfe");
}

static inline u64 esr_el1() {
  u64 esr;
  asm volatile("mrs %0, esr_el1" : "=r"(esr));

  return esr;
}

static inline void enable_irq() {
  asm volatile("msr daifclr, #2");
}

static inline void disable_irq() {
  asm volatile("msr daifset, #2");
}

#endif
