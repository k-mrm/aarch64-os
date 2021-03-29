#include "mono.h"
#include "memmap.h"
#include "gicv2.h"
#include "printk.h"

void gicv2_init() {
  REG(GICD_CTLR) = 0;

  u32 typer = REG(GICD_TYPER);
  u32 max_nint = ((typer & 0x1f) + 1) * 32;

  u32 systimer0_id = 32 + 64; /* timer0: GIC_SPI 64 */

  REG(GICD_ISENABLER(systimer0_id / 32)) |= 1 << (systimer0_id % 32);

  REG(GICD_IPRIORITYR(systimer0_id / 4)) &= ~((u32)0xff << (systimer0_id % 4 * 8));

  u32 target = 0;
  u32 itargetsr = REG(GICD_ITARGETSR(systimer0_id / 4));
  itargetsr &= ~((u32)0xff << (systimer0_id % 4 * 8));
  REG(GICD_ITARGETSR(systimer0_id / 4)) = itargetsr | ((u32)(1 << target) << (systimer0_id % 4 * 8));

  REG(GICC_CTLR) = 0x1;
  REG(GICD_CTLR) = 0x1;
}

void gic_eoi(u32 iar) {
  REG(GICC_EOIR) = iar;
}

u32 gic_iar() {
  return REG(GICC_IAR);
}
