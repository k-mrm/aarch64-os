#ifndef MONO_GICV2_H
#define MONO_GICV2_H

#define GICV2_BASE  0xff841000

#define GICD_BASE (GICV2_BASE)
#define GICC_BASE (GICV2_BASE + 0x1000)
#define GICH_BASE (GICV2_BASE + 0x3000)
#define GICV_BASE (GICV2_BASE + 0x5000)

#define GICD_CTLR (GICD_BASE)
#define GICD_TYPER  (GICD_BASE + 0x4)
#define GICD_ISENABLER(n) (GICD_BASE + 0x100 + (u64)(n) * 4)
#define GICD_IPRIORITYR(n)  (GICD_BASE + 0x400 + (u64)(n) * 4)
#define GICD_ITARGETSR(n)  (GICD_BASE + 0x800 + (u64)(n) * 4)

#define GICC_CTLR (GICC_BASE)
#define GICC_IAR  (GICC_BASE + 0xc)
#define GICC_EOIR (GICC_BASE + 0x10)
#define GICC_AIAR (GICC_BASE + 0x20)
#define GICC_AEOIR  (GICC_BASE + 0x24)

void gicv2_init(void);
void gic_eoi(u32 iar);
u32 gic_iar(void);

#endif

