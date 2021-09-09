#include "kernel.h"
#include "memmap.h"
#include "printk.h"
#include "trap.h"
#include "driver/gicv2.h"
#include "log.h"

#define GICD_BASE (GICV2_BASE)
#define GICC_BASE (GICV2_BASE + 0x10000)

#define GICD_CTLR (GICD_BASE)
#define GICD_TYPER  (GICD_BASE + 0x4)
#define GICD_ISENABLER(n) (GICD_BASE + 0x100 + (u64)(n) * 4)
#define GICD_ICENABLER(n) (GICD_BASE + 0x180 + (u64)(n) * 4)
#define GICD_ISPENDR(n) (GICD_BASE + 0x200 + (u64)(n) * 4)
#define GICD_ICPENDR(n) (GICD_BASE + 0x280 + (u64)(n) * 4)
#define GICD_IPRIORITYR(n)  (GICD_BASE + 0x400 + (u64)(n) * 4)
#define GICD_ITARGETSR(n)  (GICD_BASE + 0x800 + (u64)(n) * 4)
#define GICD_ICFGR(n)   (GICD_BASE + 0xc00 + (u64)(n) * 4)

#define GICC_CTLR (GICC_BASE)
#define GICC_PMR  (GICC_BASE + 0x4)
#define GICC_IAR  (GICC_BASE + 0xc)
#define GICC_EOIR (GICC_BASE + 0x10)
#define GICC_HPPIR  (GICC_BASE + 0x18)
#define GICC_AIAR (GICC_BASE + 0x20)
#define GICC_AEOIR  (GICC_BASE + 0x24)

enum gicd_cfg {
  GICD_CFG_LEVEL = 0,
  GICD_CFG_EDGE = 2,
};

void gic_enable_int(u32 intid) {
  REG(GICD_ISENABLER(intid / 32)) |= 1 << (intid % 32);
}

void gic_disable_int(u32 intid) {
  REG(GICD_ICENABLER(intid / 32)) |= 1 << (intid % 32);
}

void gic_set_pending(u32 intid) {
  REG(GICD_ISPENDR(intid / 32)) |= 1 << (intid % 32);
}

void gic_clear_pending(u32 intid) {
  REG(GICD_ICPENDR(intid / 32)) |= 1 << (intid % 32);
}

bool gic_is_pending(u32 intid) {
  return (REG(GICD_ISPENDR(intid / 32)) & (1 << (intid % 32))) != 0;
}

void gic_enable() {
  REG(GICC_CTLR) |= 0x1;
  REG(GICD_CTLR) |= 0x1;
}

void gic_set_prio(u32 intid, u32 prio) {
  REG(GICD_IPRIORITYR(intid / 4)) &= ~((u32)0xff << (intid % 4 * 8)); // set to 0
  // FIXME
}

void gic_set_target(u32 intid, u32 cpuid) {
  u32 itargetsr = REG(GICD_ITARGETSR(intid / 4));
  itargetsr &= ~((u32)0xff << (intid % 4 * 8));
  REG(GICD_ITARGETSR(intid / 4)) = itargetsr | ((u32)(1 << cpuid) << (intid % 4 * 8));
}

void gic_config(u32 intid, enum gicd_cfg cfg) {
  u32 icfgr = REG(GICD_ICFGR(intid / 16));
  icfgr &= ~((u32)3 << (intid % 16 * 2));
  REG(GICD_ICFGR(intid / 16)) = icfgr | ((u32)cfg << (intid % 16 * 2));
}

void gicc_init() {
  REG(GICC_CTLR) = 0;

  REG(GICC_PMR) = 0xff;
}

void gicd_init() {
  REG(GICD_CTLR) = 0;
}

void gicv2_init() {
  gicc_init();
  gicd_init();

  gic_config(TIMER_IRQ, GICD_CFG_EDGE);
  gic_set_prio(TIMER_IRQ, 0);
  gic_set_target(TIMER_IRQ, 0);
  gic_clear_pending(TIMER_IRQ);
  gic_enable_int(TIMER_IRQ);

  gic_enable();

  kinfo("gic enabled: %s\n", gic_enabled()? "true" : "false");
}

bool gic_enabled() {
  return (REG(GICC_CTLR) & 0x1) && (REG(GICD_CTLR) & 0x1);
}

void gic_eoi(u32 iar) {
  REG(GICC_EOIR) = iar;
}

u32 gic_iar() {
  return REG(GICC_IAR);
}
