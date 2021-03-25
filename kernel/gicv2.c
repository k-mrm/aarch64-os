#include "mono.h"
#include "memmap.h"
#include "gicv2.h"

void gicv2_init() {
  REG(GICD_CTLR) = 0;

  u32 typer = REG(GICD_TYPER);
  u32 max_nint = ((typer & 0x1f) + 1) * 32;

  u32 systimer0_id = 32 + 64; /* timer0: GIC_PPI 13 */

  REG(GICD_ISENABLER(systimer0_id / 32)) |= 1 << (systimer0_id % 32);

  REG(GICD_IPRIORITYR(systimer0_id / 4)) &= ~((u32)0xff << (systimer0_id % 4 * 8));

  REG(GICD_ITARGETSR(systimer0_id / 4)) |= (u32)(1 << 0) << (systimer0_id % 4 * 8);

  REG(GICD_CTLR) |= 0x1;
  REG(GICC_CTLR) |= 0x1;
}
