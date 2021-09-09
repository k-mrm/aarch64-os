#ifndef AARCH64_OS_AARCH64_H
#define AARCH64_OS_AARCH64_H

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

static inline void wfi() {
  asm volatile("wfi");
}

static inline void isb() {
  asm volatile("isb");
}

static inline u64 elr_el1() {
  u64 elr;
  asm volatile("mrs %0, elr_el1" : "=r"(elr));

  return elr;
}

static inline u64 esr_el1() {
  u64 esr;
  asm volatile("mrs %0, esr_el1" : "=r"(esr));

  return esr;
}

static inline u64 spsr_el1() {
  u64 spsr;
  asm volatile("mrs %0, spsr_el1" : "=r"(spsr));

  return spsr;
}

static inline u64 far_el1() {
  u64 far;
  asm volatile("mrs %0, far_el1" : "=r"(far));

  return far;
}

static inline void enable_irq() {
  asm volatile("msr daifclr, #2");
}

static inline void disable_irq() {
  asm volatile("msr daifset, #2");
}

static inline u64 daif() {
  u64 d;
  asm volatile("mrs %0, daif" : "=r"(d));
  return d;
}

static inline u64 mpidr_el1() {
  u64 m;
  asm volatile("mrs %0, mpidr_el1" : "=r"(m));
  return m;
}

static inline u64 cntv_ctl_el0() {
  u64 c;
  asm volatile("mrs %0, cntv_ctl_el0" : "=r"(c));
  return c;
};

static inline void set_cntv_ctl_el0(u64 c) {
  asm volatile("msr cntv_ctl_el0, %0" : : "r"(c));
};

static inline u64 cntv_tval_el0() {
  u64 t;
  asm volatile("mrs %0, cntv_tval_el0" : "=r"(t));
  return t;
}

static inline void set_cntv_tval_el0(u64 t) {
  asm volatile("msr cntv_tval_el0, %0" : : "r"(t));
}

static inline u64 cntv_cval_el0() {
  u64 c;
  asm volatile("mrs %0, cntv_cval_el0" : "=r"(c));
  return c;
}

static inline void set_cntv_cval_el0(u64 c) {
  asm volatile("msr cntv_cval_el0, %0" : : "r"(c));
}

static inline u64 cntvct_el0() {
  u64 c;
  asm volatile("mrs %0, cntvct_el0" : "=r"(c));
  return c;
};

static inline u64 cntfrq_el0() {
  u64 f;
  asm volatile("mrs %0, cntfrq_el0" : "=r"(f));
  return f;
}

static inline void set_ttbr0_el1(u64 t) {
  asm volatile("msr ttbr0_el1, %0" : : "r"(t));
}

static inline void set_ttbr1_el1(u64 t) {
  asm volatile("msr ttbr1_el1, %0" : : "r"(t));
}

static inline void set_tcr_el1(u64 t) {
  asm volatile("msr tcr_el1, %0" : : "r"(t));
}

#endif
