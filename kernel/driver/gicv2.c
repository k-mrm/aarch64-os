#include "mono.h"
#include "memmap.h"
#include "printk.h"
#include "driver/gicv2.h"

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

#define GICC_CTLR (GICC_BASE)
#define GICC_PMR  (GICC_BASE + 0x4)
#define GICC_IAR  (GICC_BASE + 0xc)
#define GICC_EOIR (GICC_BASE + 0x10)
#define GICC_HPPIR  (GICC_BASE + 0x18)
#define GICC_AIAR (GICC_BASE + 0x20)
#define GICC_AEOIR  (GICC_BASE + 0x24)

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
  REG(GICC_CTLR) = 0x1;
  REG(GICD_CTLR) = 0x1;
}

void gicv2_init() {
  printk("gicv2 init base: %p\n", GICD_BASE);

  REG(GICD_CTLR) = 0;

  u32 typer = REG(GICD_TYPER);
  u32 max_nint = ((typer & 0x1f) + 1) * 32;

  /* FIXME */
  u32 archtimer_id = 27; /* arch timer intid: 27 */

  gic_enable_int(archtimer_id);

  REG(GICD_IPRIORITYR(archtimer_id / 4)) &= ~((u32)0xff << (archtimer_id % 4 * 8));

  /*
  u32 target = 0;
  u32 itargetsr = REG(GICD_ITARGETSR(archtimer_id / 4));
  itargetsr &= ~((u32)0xff << (archtimer_id % 4 * 8));
  REG(GICD_ITARGETSR(archtimer_id / 4)) = itargetsr | ((u32)(1 << target) << (archtimer_id % 4 * 8));
  */

  gic_enable();

  printk("gic enabled: %s\n", gic_enabled()? "true" : "false");
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
