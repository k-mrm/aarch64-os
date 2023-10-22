#include "aarch64.h"
#include "proc.h"
#include "spinlock.h"
#include "printk.h"

void pushcli();
void popcli();

bool holding(struct spinlock *lk) {
  return lk->lock && lk->cpuid == cpuid();
}

void acquire(struct spinlock *lk) {
  u8 tmp, l = 1;

  pushcli();

  if(holding(lk))
    panic("already held %d", lk->cpuid);

  asm volatile(
    "sevl\n"
    "1: wfe\n"
    "2: ldaxrb %w0, [%1]\n"
    "cbnz   %w0, 1b\n"
    "stxrb  %w0, %w2, [%1]\n"
    "cbnz   %w0, 2b\n"
    : "=&r"(tmp) : "r"(&lk->lock), "r"(l) : "memory"
  );

  isb();

  lk->cpuid = cpuid();
}

void release(struct spinlock *lk) {
  if(!holding(lk))
    panic("invalid release");

  lk->cpuid = -1;

  isb();

  asm volatile("stlrb wzr, [%0]" :: "r"(&lk->lock) : "memory");

  popcli();
}

void pushcli() {
  struct cpu *cpu = mycpu();

  if(cpu->cli_depth++ == 0) {
    cpu->intr_enabled = irq_enabled();
    disable_irq();
  }
}

void popcli() {
  struct cpu *cpu = mycpu();

  if(cpu->cli_depth == 0)
    panic("popcli");

  if(--cpu->cli_depth == 0) {
    if(cpu->intr_enabled)
      enable_irq();
  }
}

void lock_init(struct spinlock *lk) {
  lk->lock = 0;
  lk->cpuid = -1;
}
